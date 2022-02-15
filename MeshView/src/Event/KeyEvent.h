#pragma once

#include"Event.h"
#include<sstream>

namespace MV
{
	class KeyEvent :public Event
	{
	public:
		KeyEvent(int keycode) :
			m_KeyCode(keycode) {}

		virtual ~KeyEvent() = default;

		int GetCategory()const override
		{
			return ( EventCategoryKeyboard| EventCategoryInput);
		}

		inline int GetKeyCode()const { return m_KeyCode; }
	private:
		int m_KeyCode;
	};


	class KeyPressedEvent :public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode,int repeatcount):
			KeyEvent(keycode),m_RepeatCount(repeatcount){}
		inline int GetRepeatCount() const { return m_RepeatCount; }
		EventType GetType()const override
		{
			return GetEventType();
		}

		static EventType GetEventType() { return EventType::KeyPressed; }


		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent:" << KeyEvent::GetKeyCode()<<" "<< m_RepeatCount;
			return ss.str();
		}

	
	private:
		int m_RepeatCount;

	};


	class KeyReleasedEvent :public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode) :
			KeyEvent(keycode) {}

		EventType GetType()const override
		{
			return GetEventType();
		}

		static EventType GetEventType() { return EventType::KeyReleased; }


		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent:" << KeyEvent::GetKeyCode();
			return ss.str();
		}

	};

	class KeyTypedEvent :public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode) :
			KeyEvent(keycode) {}

		EventType GetType()const override
		{
			return GetEventType();
		}

		static EventType GetEventType() { return EventType::KeyTyped; }


		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent:" << (char)KeyEvent::GetKeyCode();
			return ss.str();
		}

	};
}
