#pragma once
#include"Macro.h"
#include<spdlog/spdlog.h>
#include"spdlog/sinks/base_sink.h"
namespace MV
{

	class Log
	{
	public:
		inline static Ref<spdlog::logger> GetLogger() { return m_Logger; }
		static void Init();
	private:
		static Ref<spdlog::logger> m_Logger;
	};
}

#define MESH_TRACE(...) MV::Log::GetLogger()->trace(__VA_ARGS__) 
#define MESH_INFO(...) MV::Log::GetLogger()->info(__VA_ARGS__) 
#define MESH_WARN(...) MV::Log::GetLogger()->warn(__VA_ARGS__) 
#define MESH_ERROR(...) MV::Log::GetLogger()->error(__VA_ARGS__) 