#pragma once
#include"Event.h"
#include<sstream>

namespace MV
{
	class WindowClosedEvent:public Event
	{
	public:
		WindowClosedEvent() = default;

		int GetCategory()const override
		{
			return (EventCategoryAppication);
		}
		EventType GetType()const override
		{
			return GetEventType();
		}

		static EventType GetEventType() { return EventType::WindowClosed; }


		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "WindowClosedEvent";
			return ss.str();
		}
	};


	class WindowResizedEvent :public Event
	{
	public:

		WindowResizedEvent(uint32_t width,uint32_t height)
			:m_Width(width),m_Height(height){}

		int GetCategory()const override
		{
			return (EventCategoryAppication);
		}
		EventType GetType()const override
		{
			return GetEventType();
		}

		static EventType GetEventType() { return EventType::WindowResized; }


		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "WindowResizedEvent:" << " width: " << m_Width << ","
				"height: " << m_Height;
			return ss.str();
		}
	private:
		uint32_t m_Width, m_Height;
		
	};

	class WindowDropEvent :public Event
	{
	public:
		WindowDropEvent() = default;
		int GetCategory()const override
		{
			return (EventCategoryAppication);
		}

		EventType GetType()const override
		{
			return GetEventType();
		}

		static EventType GetEventType() { return EventType::WindowDrop; }

		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "WindwoResizeEvent: files:" << m_FileName.size() << "\n";
			for (auto& f : m_FileName)
			{
				ss << f << "\n";
			}
			return ss.str();
		}

		void PushFile(const std::string& file)
		{
			m_FileName.push_back(file);
		}
		void PushFile(std::string&& file)
		{
			m_FileName.emplace_back(std::move(file));
		}

		const std::vector<std::string>& GetFiles()const { return m_FileName; }
	private:
		std::vector<std::string> m_FileName;
	};
}