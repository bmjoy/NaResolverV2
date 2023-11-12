#include <Windows.h>
#include "../NaResolver.h"

using namespace NaOrganization::MidTerm;

class Object
{
public:
	CLASS("UnityEngine.CoreModule", "UnityEngine", "Object");

	int GetInstanceID()
	{
		static auto invoker = NaApiInvoker<int, void*>(UnityResolver.GetMethod(ThisClass(), "System.Int32", "GetInstanceID", {})
			.method.GetInvokeAddress());
		return invoker.Invoke(this);
	}

	void set_name(UnityGeneral::String value)
	{
		static auto invoker = NaApiInvoker<void, void*, UnityGeneral::String>(UnityResolver.GetMethod(ThisClass(), "System.Void", "set_name", { "System.String" })
			.method.GetInvokeAddress());
		invoker.Invoke(this, value);
	}
};

class GameObject : public Object
{
public:
	static NaResolver::Class ThisClass()
	{
		return UnityResolver.GetClass("UnityEngine.CoreModule", "UnityEngine", "GameObject");
	}

	void* get_transform()
	{
		static auto invoker = NaApiInvoker<void*, void*>(UnityResolver.GetMethod(ThisClass(), "UnityEngine.Transform", "get_transform", {})
			.method.GetInvokeAddress());
		return invoker.Invoke(this);
	}

	UnityGeneral::String get_tag()
	{
		static auto invoker = NaApiInvoker<void*, void*>(UnityResolver.GetMethod(ThisClass(), "UnityEngine.Transform", "get_tag", {})
			.method.GetInvokeAddress());
		return invoker.Invoke(this);
	}
};


int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hinstDLL);
		AllocConsole();
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

		try
		{
			if (!UnityResolver.Setup())
				printf("Failed to setup I2Hrame!\n");
		}
		catch (const NaResolver::Exception& e)
		{
			printf("[NaResolver] %s\n", e.message.c_str());
		}

		NaResolver::Class gameObject = GameObject::ThisClass();
		printf("(NaResolver->GetClass) Class(%p): %p\n", gameObject.type, gameObject.klass);
		NaResolver::Class object = Object::ThisClass();
		printf("(NaResolver->GetClass) Class(%p): %p\n", object.type, object.klass);
		NaResolver::Method get_transform = UnityResolver.GetMethod(gameObject, "UnityEngine.Transform", "get_transform", {});
		printf("(NaResolver->GetMethod) Method(%p): %p\n", get_transform.method.GetInvokeAddress(), get_transform.method);
		void* __this = nullptr; // fake value for test
		void* result = NaApiInvoker<void*, void*>(get_transform.method.GetInvokeAddress()).Invoke(__this);

		Sleep(5000);
		UnityResolver.Destroy();
		FreeConsole();
		FreeLibrary(hinstDLL);
	}
	return TRUE; // Successful DLL_PROCESS_ATTACH.
}
