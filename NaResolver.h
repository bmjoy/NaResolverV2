//**************************************//
// Hi NaResolver						//
// Author: MidTerm						//
// Version: v2.0						//
// License: MIT							//
//**************************************//

// Change Log (Started since v1.8):
// Release v2.0:
// 1. Remake most of codes
// 2. Support the mono and il2cpp in only one version
// 3. Remove that enforce cpp version requirements
// 4. Remove the structure of signature
// Release v1.8:
// 1. Add pre-register mechanism, all class, method and field must register
// 2. Add a exception class
//

#undef GetClassName

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <codecvt>

#ifndef H_NARESOLVER
#define H_NARESOLVER

#ifndef NA_RESOLVER_STRING_XOR
#define NA_RESOLVER_STRING_XOR
#endif

#undef RegisterClass
#undef TEXT

#ifdef _HAS_CXX17
#define CXX17_INLINE inline
#else
#define CXX17_INLINE
#endif

#define TEXT NA_RESOLVER_STRING_XOR

namespace NaOrganization
{
	namespace MidTerm
	{
		template<typename R, typename ...Args>
		class NaApiInvoker
		{
		public:
			using functionPtrType = R(*)(Args...);
			functionPtrType content = {};

			NaApiInvoker() {}
			NaApiInvoker(void* address) : content((functionPtrType)address) {}

			template<typename T = R,
				typename std::enable_if<!std::is_void<T>::value, int>::type = 0>
			R Invoke(Args ... arg) const
			{
				R result = {};
				if (content != nullptr)
				{
					result = content(arg...);
				}
				return result;
			}

			template<typename T = R,
				typename std::enable_if<std::is_void<T>::value, int>::type = 0>
			void Invoke(Args ... arg) const
			{
				if (content != nullptr)
				{
					content(arg...);
				}
			}

			R operator()(Args ... arg) const
			{
				return Invoke(arg...);
			}

			bool IsValid() const
			{
				return content != nullptr;
			}
		};

		namespace UnityGeneral
		{
			template<typename R, typename ...Args>
			class UnityApiInvoker : public NaApiInvoker<R, Args...>
			{
			public:
				UnityApiInvoker(std::string symbol)
				{
					this->content = (R(*)(Args...))GetProcAddress(GetModule(), symbol.c_str());
				}
				UnityApiInvoker(std::string il2CppSymbol, std::string monoSymbol)
				{
					this->content = (R(*)(Args...))GetProcAddress(GetModule(), il2CppSymbol.c_str());
					if (this->content == nullptr)
					{
						this->content = (R(*)(Args...))GetProcAddress(GetModule(), monoSymbol.c_str());
					}
				}

				static HMODULE GetModule();
			};

			class String
			{
			public:
				void* address = nullptr;
				wchar_t* data = nullptr;
				String() {}
				String(void* address) : address(address)
				{
					static auto string_chars = UnityApiInvoker<wchar_t*, void*>(TEXT("il2cpp_string_chars"), TEXT("mono_string_chars"));
					data = string_chars(address);
				}
				String(std::string string)
				{
					// il2cpp_string_new(x), mono_string_new(domain, x)
					static auto il2cpp_string_new = UnityApiInvoker<void*, const char*>(TEXT("il2cpp_string_new"));
					if (il2cpp_string_new.IsValid())
					{
						address = il2cpp_string_new(string.c_str());
					}
					else
					{
						static auto mono_string_new = UnityApiInvoker<void*, void*, const char*>(TEXT("mono_string_new"));
						address = mono_string_new(Domain::Get(), string.c_str());
					}
				}
				operator void* const () { return address; }

				std::string ToString() const
				{
					if (!data)
						return "";
					std::wstring wData(data);
					return std::string(wData.begin(), wData.end());
				}

				operator std::string const () { return ToString(); }
			};

			class Type
			{
			public:
				void* type = NULL;
				Type() {}
				Type(void* type) : type(type) {}
				operator void* const () { return type; }

				std::string GetName() const;
			};

			class Method
			{
			public:
				void* method = NULL;
				Method() {}
				Method(void* method) : method(method) {}
				operator void* const () { return method; }

				void* GetSignatureForMono() const;

				std::string GetName() const;

				Type GetReturnType() const;

				std::vector<Type> GetParametersType() const;

				void* GetInvokeAddress() const;
			};

			class Class
			{
			public:
				void* klass = NULL;
				Class() {}
				Class(void* klass) : klass(klass) {}
				operator void* const () { return klass; }

				Method GetMethods(void** iter) const;
			};

			class Image
			{
			public:
				void* image = NULL;
				Image() {}
				Image(void* image) : image(image) {}
				operator void* const () { return image; }

				std::string GetName() const;

				Class GetClassFromName(std::string namespaceName, std::string className) const;
			};

			class Assembly
			{
			public:
				void* assembly = nullptr;
				Assembly() {}
				Assembly(void* assembly) : assembly(assembly) {}
				operator void* const () { return assembly; }

				Image GetImage() const;
			};

			class Domain
			{
			public:
				void* domain = nullptr;
				Domain() {}
				Domain(void* domain) : domain(domain) {}
				operator void* const () { return domain; }

				static Domain Get();

				Assembly OpenAssembly(std::string name) const;
			};

			class Thread
			{
			public:
				CXX17_INLINE static void* alreadyAttach;
				void* thread = NULL;
				Thread() {}
				Thread(void* thread) : thread(thread) {}
				operator void* const () { return thread; }

				static Thread Attach(Domain domain);

				static Thread Current();

				void Detach() const;
			};
		}

		class NaResolver
		{
		public:
			class Class
			{
			public:
				std::string assemblyName = std::string();
				std::string namespaceName = std::string();
				std::string className = std::string();
				UnityGeneral::Class klass = UnityGeneral::Class();
				UnityGeneral::Type type = UnityGeneral::Type();
				Class() {}
				Class(std::string assemblyName, std::string namespaceName, std::string className, UnityGeneral::Class klass, UnityGeneral::Type type) : assemblyName(assemblyName), namespaceName(namespaceName), className(className), klass(klass), type(type) {}
				operator UnityGeneral::Class() { return klass; }
				operator UnityGeneral::Type() { return type; }
				operator bool() { return klass && type; }
			};
			class Method
			{
			public:
				std::string returnTypeName = std::string();
				std::string methodName = std::string();
				std::vector<std::string> parametersTypeName = std::vector<std::string>();
				UnityGeneral::Method method = UnityGeneral::Method();
				Method() {}
				Method(std::string returnTypeName, std::string methodName, std::vector<std::string> parametersTypeName, UnityGeneral::Method method) : returnTypeName(returnTypeName), methodName(methodName), parametersTypeName(parametersTypeName), method(method) {}
				operator UnityGeneral::Method() { return method; }
				operator bool() { return method; }
			};
			class Exception
			{
			public:
				enum class Level
				{
					Fatal,
					Error,
					None
				};
				Level level = Level::None;
				std::string message = std::string();

				Exception(Level level, std::string message, ...);
			};
			class ContextCache
			{
			public:
				using AssemblyMap = std::unordered_map<std::string, UnityGeneral::Assembly>;
				using ClassPathMap = std::unordered_map<std::string,	// Assembly
					std::unordered_map<std::string,						// Namespace
					std::unordered_map<std::string, Class>			// Class
					>
				>;
			private:
				AssemblyMap assemblyMap = {};
				ClassPathMap classPathMap = {};
			public:
				ContextCache() {}

				void RegisterAssembly(std::string name, UnityGeneral::Assembly assembly);

				Class RegisterClass(std::string assembly, std::string nameSpace, std::string name, UnityGeneral::Class klass, UnityGeneral::Type type);

				UnityGeneral::Assembly GetAssembly(std::string name) const;

				Class GetClass(std::string assembly, std::string nameSpace, std::string name) const;

				void Clear();
			};
		private:
			UnityGeneral::Domain domain = UnityGeneral::Domain();
			UnityGeneral::Thread thread = UnityGeneral::Thread();
			ContextCache cache = ContextCache();
		public:
			bool Setup();
			void Destroy();
			Class GetClass(std::string assemblyName, std::string namespaceName, std::string className);
			Method GetMethod(Class parent, std::string returnTypeName, std::string methodName, std::vector<std::string> parametersTypeName);
		};

		CXX17_INLINE NaResolver UnityResolver = NaResolver();

		template<typename R, typename ...Args>
		HMODULE UnityGeneral::UnityApiInvoker<R, Args...>::GetModule()
		{
			static HMODULE moduleHandle = NULL;
			moduleHandle = GetModuleHandleA(TEXT("GameAssembly.dll"));
			if (moduleHandle != NULL)
				return moduleHandle;
			moduleHandle = GetModuleHandleA(TEXT("mono-2.0-bdwgc.dll"));
			if (moduleHandle != NULL)
				return moduleHandle;
			return moduleHandle;
		}

		std::string UnityGeneral::Type::GetName() const
		{
			static auto type_get_name = UnityApiInvoker<const char*, void*>(TEXT("il2cpp_type_get_name"), TEXT("mono_type_get_name"));
			return type_get_name(type);
		}

		void* UnityGeneral::Method::GetSignatureForMono() const
		{
			static auto method_get_signature = UnityApiInvoker<void*, void*>(TEXT("mono_method_signature"));
			return method_get_signature(method);
		}

		std::string UnityGeneral::Method::GetName() const
		{
			static auto method_get_name = UnityApiInvoker<const char*, void*>(TEXT("il2cpp_method_get_name"), TEXT("mono_method_get_name"));
			return method_get_name(method);
		}

		UnityGeneral::Type UnityGeneral::Method::GetReturnType() const
		{
			static auto method_get_return_type = UnityApiInvoker<void*, void*>(TEXT("il2cpp_method_get_return_type"));
			if (method_get_return_type.IsValid())
			{
				return method_get_return_type(method);
			}
			static auto mono_signature_get_return_type = UnityApiInvoker<void*, void*>(TEXT("mono_signature_get_return_type"));
			if (mono_signature_get_return_type.IsValid())
			{
				return mono_signature_get_return_type(GetSignatureForMono());
			}
			return Type();
		}

		std::vector<UnityGeneral::Type> UnityGeneral::Method::GetParametersType() const
		{
			std::vector<Type> types = {};
			static auto method_get_param_count = UnityApiInvoker<uint32_t, void*>(TEXT("il2cpp_method_get_param_count"));
			static auto method_get_param = UnityApiInvoker<void*, void*, uint32_t>(TEXT("il2cpp_method_get_param"));
			if (method_get_param_count.IsValid() && method_get_param.IsValid())
			{
				uint32_t count = method_get_param_count(method);
				for (uint32_t i = 0; i < count; i++)
				{
					types.push_back(method_get_param(method, i));
				}
			}
			static auto mono_signature_get_params = UnityApiInvoker<void*, void*, void*>(TEXT("mono_signature_get_params"));
			if (mono_signature_get_params.IsValid())
			{
				void* iter = NULL;
				void* param = NULL;
				while ((param = mono_signature_get_params(GetSignatureForMono(), &iter)) != NULL)
				{
					types.push_back(param);
				}
			}
			return types;
		}

		void* UnityGeneral::Method::GetInvokeAddress() const
		{
			static auto mono_compile_method = UnityApiInvoker<void*, void*>(TEXT("mono_compile_method"));
			if (mono_compile_method.IsValid())
			{
				Thread current = Thread::Current();
				if (current == nullptr)
					return nullptr;
				if (Thread::alreadyAttach != current)
					throw std::exception(TEXT("Do not use the GetInvokeAddress in diffrent with the Application's setup thread!"));
				return mono_compile_method(method);
			}
			return *(void**)method;
		}

		UnityGeneral::Method UnityGeneral::Class::GetMethods(void** iter) const
		{
			static auto class_get_methods = UnityApiInvoker<void*, void*, void**>(TEXT("il2cpp_class_get_methods"), TEXT("mono_class_get_methods"));
			return class_get_methods(klass, iter);
		}

		std::string UnityGeneral::Image::GetName() const
		{
			static auto image_get_name = UnityApiInvoker<const char*, void*>(TEXT("il2cpp_image_get_name"), TEXT("mono_image_get_name"));
			return image_get_name(image);
		}

		UnityGeneral::Class UnityGeneral::Image::GetClassFromName(std::string namespaceName, std::string className) const
		{
			static auto class_from_name = UnityApiInvoker<void*, void*, const char*, const char*>(TEXT("il2cpp_class_from_name"), TEXT("mono_class_from_name"));
			return class_from_name(image, namespaceName.c_str(), className.c_str());
		}

		UnityGeneral::Image UnityGeneral::Assembly::GetImage() const
		{
			static auto assembly_get_image = UnityApiInvoker<void*, void*>(TEXT("il2cpp_assembly_get_image"), TEXT("mono_assembly_get_image"));
			return assembly_get_image(assembly);
		}

		UnityGeneral::Domain UnityGeneral::Domain::Get()
		{
			static auto domain_get = UnityApiInvoker<void*>(TEXT("il2cpp_domain_get"), TEXT("mono_get_root_domain"));
			return domain_get();
		}

		UnityGeneral::Assembly UnityGeneral::Domain::OpenAssembly(std::string name) const
		{
			static auto assembly_get = UnityApiInvoker<void*, void*, const char*>(TEXT("il2cpp_domain_assembly_open"), TEXT("mono_domain_assembly_open"));
			return assembly_get(domain, name.c_str());
		}

		UnityGeneral::Thread UnityGeneral::Thread::Attach(Domain domain)
		{
			static auto thread_attach = UnityApiInvoker<void*, void*>(TEXT("il2cpp_thread_attach"), TEXT("mono_thread_attach"));
			return alreadyAttach = thread_attach(domain);
		}

		UnityGeneral::Thread UnityGeneral::Thread::Current()
		{
			static auto thread_current = UnityApiInvoker<void*>(TEXT("il2cpp_thread_current"), TEXT("mono_thread_current"));
			return thread_current();
		}

		void UnityGeneral::Thread::Detach() const
		{
			static auto thread_detach = UnityApiInvoker<void, void*>(TEXT("il2cpp_thread_detach"), TEXT("mono_thread_detach"));
			thread_detach(thread);
		}

		NaResolver::Exception::Exception(Level level, std::string message, ...) : level(level)
		{
			va_list args;
			va_start(args, message);
			int size = _vscprintf(message.c_str(), args) + 1;
			char* buf = new char[size];
			vsprintf_s(buf, size, message.c_str(), args);
			va_end(args);
			this->message = buf;
			delete[] buf;
		}

		void NaResolver::ContextCache::RegisterAssembly(std::string name, UnityGeneral::Assembly assembly)
		{
			assemblyMap[name] = assembly;
		}

		NaResolver::Class NaResolver::ContextCache::RegisterClass(std::string assembly, std::string nameSpace, std::string name, UnityGeneral::Class klass, UnityGeneral::Type type)
		{
			return classPathMap[assembly][nameSpace][name] = Class(assembly, nameSpace, name, klass, type);
		}

		UnityGeneral::Assembly NaResolver::ContextCache::GetAssembly(std::string name) const
		{
			auto iter = assemblyMap.find(name);
			if (iter == assemblyMap.end())
				return UnityGeneral::Assembly();
			return iter->second;
		}

		NaResolver::Class NaResolver::ContextCache::GetClass(std::string assembly, std::string nameSpace, std::string name) const
		{
			auto assemblyIter = classPathMap.find(assembly);
			if (assemblyIter == classPathMap.end())
				return Class();
			auto nameSpaceIter = assemblyIter->second.find(nameSpace);
			if (nameSpaceIter == assemblyIter->second.end())
				return Class();
			auto nameIter = nameSpaceIter->second.find(name);
			if (nameIter == nameSpaceIter->second.end())
				return Class();
			return nameIter->second;
		}

		void NaResolver::ContextCache::Clear()
		{
			assemblyMap.clear();
			classPathMap.clear();
		}

		bool NaResolver::Setup()
		{
			domain = UnityGeneral::Domain::Get();
			if (domain == nullptr)
			{
				throw Exception(Exception::Level::Fatal, TEXT("NaResolver::Setup() : Can not get the domain!"));
				return false;
			}
			thread = UnityGeneral::Thread::Attach(domain);
			if (thread == nullptr)
			{
				throw Exception(Exception::Level::Fatal, TEXT("NaResolver::Setup() : Can not attach the thread!"));
				return false;
			}
			return true;
		}

		void NaResolver::Destroy()
		{
			thread.Detach();
			cache.Clear();
		}

		NaResolver::Class NaResolver::GetClass(std::string assemblyName, std::string namespaceName, std::string className)
		{
			Class result = cache.GetClass(assemblyName, namespaceName, className);
			if (result)
				return result;
			UnityGeneral::Assembly assembly = cache.GetAssembly(assemblyName);
			if (!assembly)
			{
				assembly = domain.OpenAssembly(assemblyName);
				if (!assembly)
				{
					throw Exception(Exception::Level::Error, TEXT("NaResolver::GetClass() : Can not open the assembly \"%s\"!"), assemblyName.c_str());
					return Class();
				}
				cache.RegisterAssembly(assemblyName, assembly);
			}
			UnityGeneral::Image image = assembly.GetImage();
			if (!image)
			{
				throw Exception(Exception::Level::Error, TEXT("NaResolver::GetClass() : Can not get the image from the assembly \"%s\"!"), assemblyName.c_str());
				return Class();
			}
			UnityGeneral::Class klass = image.GetClassFromName(namespaceName, className);
			if (!klass)
			{
				throw Exception(Exception::Level::Error, TEXT("NaResolver::GetClass() : Can not get the class \"%s\" from the assembly \"%s\"!"), className.c_str(), assemblyName.c_str());
				return Class();
			}
			return result = cache.RegisterClass(assemblyName, namespaceName, className, klass, UnityGeneral::Type(klass));
		}

		NaResolver::Method NaResolver::GetMethod(Class parent, std::string returnTypeName, std::string methodName, std::vector<std::string> parametersTypeName)
		{
			if (!parent)
			{
				throw Exception(Exception::Level::Error, TEXT("NaResolver::GetMethod() : The parent class is invalid!"));
				return NaResolver::Method();
			}
			void* iterator = nullptr;
			UnityGeneral::Method method = nullptr;
			while ((method = parent.klass.GetMethods(&iterator)) != nullptr)
			{
				std::string name = method.GetName();
				if (name.compare(methodName) == std::string::npos && name.compare(TEXT("_AUTO_")) == std::string::npos)
					continue;
				std::string returnType = method.GetReturnType().GetName();
				if (returnType.compare(returnTypeName) != std::string::npos && returnType.compare(TEXT("_AUTO_")) != std::string::npos)
					continue;
				std::vector<UnityGeneral::Type> parametersType = method.GetParametersType();
				if (parametersType.size() != parametersTypeName.size())
					continue;
				bool isMatch = true;
				for (size_t i = 0; i < parametersType.size(); i++)
				{
					std::string parameterType = parametersType[i].GetName();
					if (parameterType.compare(parametersTypeName[i]) != std::string::npos && parameterType.compare(TEXT("_AUTO_")) != std::string::npos)
					{
						isMatch = false;
						break;
					}
				}
				if (!isMatch)
					continue;
				return Method(returnType, name, parametersTypeName, method);
			}
			return NaResolver::Method();
		}
	}
}

#undef TEXT

#define CLASS(assemblyName,namespaceName,className) \
	static NaResolver::Class ThisClass() \
	{ \
		return UnityResolver.GetClass(assemblyName, namespaceName, className); \
	}

#endif // !H_NARESOLVER