#  NaResolver V2

"Lightweight UnityGame plugin development framework，Give you a excellent experience."

------

| [Introduction](#Introduction) - [The Pitch](#The-Pitch) - [Usage](#Usage) - [License](#License) |
| :----------------------------------------------------------: |

### Introduction

**NaResolver** is a lightweight development framework for plugin of UnityGame from the **Perfect Team**'s sub project **"NaLibrary"**

**NaResolver** will not use the MIT as a licence since the **v2.0**



### The Pitch

**NaResolver** is a lightweight development  framework for plugin of UnityGame written in C++ language

**NaResolver** has excellent compatibility

- Fast setup
- Excellent compatibility
- Support x64 and x86
- Support Mono and Il2Cpp  **(New)**

### Usage

####  How to setup？

```cpp
#define NA_RESOLVER_STRING_XOR _xor_ //if you wanna use _xor_
#include <NaResolver/NaResolver.h>

using namespace NaOrganization::MidTerm;

...main()
{
	if (UnityResolver.Setup())
        printf("Setup successfully!");
}
```

#### How to get class, method？

```cpp
// class
NaResolver::Class* klass = UnityResolver.GetClass("assembly", "namespace", "className");

// method
UnityResolver.GetMethod(klass, "returnType", "methodName", {"parameterType1", "parameterType2"});
```

#### How to use them

```cpp
// class
printf("(NaResolver->GetClass) Class(%p): %p\n", gameObject.type, gameObject.klass);

// method
printf("(NaResolver->GetMethod) Method(%p): %p\n", get_transform.method.GetInvokeAddress(), get_transform.method);
void* __this = nullptr; // fake value for test
void* result = NaApiInvoker<void*, void*>(get_transform.method.GetInvokeAddress()).Invoke(__this);
```

## License

**NaResolver** is licensed under the GPL3.0 License, see [LICENSE.txt](LICENSE.txt) for more information.