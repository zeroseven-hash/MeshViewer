#include"pch.h"
#include "LogPanel.h"

namespace MV
{
	LogPanel::LogPanel()
	{
		m_Sink = std::make_shared<my_sink_st>();
		Init();
	}

	LogPanel::LogPanel(Layer* context)
		:m_Context(context)
	{
		m_Sink = std::make_shared<my_sink_st>();
		Init();
	}

	LogPanel::~LogPanel()
	{
		auto iter = std::find(Log::GetLogger()->sinks().begin(), Log::GetLogger()->sinks().end(), m_Sink);
		Log::GetLogger()->sinks().erase(iter);
	}

	void LogPanel::OnImGuiRender()
	{
		m_Sink->GetLog().Draw("Log Console", nullptr);
	}

	void LogPanel::Init()
	{
		m_Sink->set_level(spdlog::level::info);
		m_Sink->set_pattern("[%R]: %v");
		Log::GetLogger()->sinks().push_back(m_Sink);
	}
}
