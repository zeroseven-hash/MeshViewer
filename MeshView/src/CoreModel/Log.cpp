#include"pch.h"
#include"Log.h"
#include"spdlog/sinks/stdout_color_sinks.h"
namespace MV
{

	Ref<spdlog::logger> Log::m_Logger;
	void Log::Init()
	{
		m_Logger = spdlog::stdout_color_mt("IGameLogger");
		m_Logger->set_level(spdlog::level::trace);
	}


}