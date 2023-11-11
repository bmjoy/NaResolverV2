#  NaResolver V2

"轻量化的Unity游戏插件开发框架，给你带来极佳的体验."

------

| [介绍](#介绍) - [特点](#特点) - [如何使用](#如何使用) - [协议](#协议) |
| :----------------------------------------------------------: |
|                [English Readme](README.en.md)                |

### 介绍

**NaResolver**是为插件开发研发的一款轻量化开发框架. 

自**v2.0**起**NaResolver**将不在使用MIT协议开源

### 特点

"NaResolver"是一个由C++语言编写的轻量化的UnityEngine游戏插件项目框架

"NaResolver"有强劲兼容性

- 快速初始化
- 较好的兼容性
- 支持x64与x86
- 同时支持Mono与Il2Cpp **(New)**

### 如何使用

#### 如何安装框架呢？

```cpp
#define NA_RESOLVER_STRING_XOR _xor_ //如果你需要使用xor那么
#include <NaResolver/NaResolver.h>

using namespace NaOrganization::MidTerm;

...main()
{
	if (UnityResolver.Setup())
        printf("Setup successfully!");
}
```
#### 要如何获取类和方法呢？

```cpp
// 获取类
NaResolver::Class* klass = UnityResolver.GetClass("assembly", "namespace", "className");

// 获取方法
UnityResolver.GetMethod(klass, "returnType", "methodName", {"parameterType1", "parameterType2"});
```
#### 要如何使用呢？
```cpp
// 类
printf("(NaResolver->GetClass) Class(%p): %p\n", gameObject.type, gameObject.klass);

// 方法
printf("(NaResolver->GetMethod) Method(%p): %p\n", get_transform.method.GetInvokeAddress(), get_transform.method);
void* __this = nullptr; // fake value for test
void* result = NaApiInvoker<void*, void*>(get_transform.method.GetInvokeAddress()).Invoke(__this);
```
协议
-------

**NaResolver**使用GPL3.0协议, 详细见 [LICENSE.txt](LICENSE.txt).
