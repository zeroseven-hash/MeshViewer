#pragma once



#define PLATFOR_WINDOWS



#define IGAME_ASSERT(x,...) {if(!(x)) {MESH_ERROR("Assertion Failed: {0}",__VA_ARGS__);__debugbreak();}}


#define BIT(x) (1<<x)

#include"KeyCode.h"
#include<memory>

namespace MV
{
	template<typename T>
	using Unique = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr <T>;
}