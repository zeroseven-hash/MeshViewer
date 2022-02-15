#pragma once
#include<sstream>
#include"Event.h"
namespace MV
{
	class MouseMovedEvent:public Event
	{

	public:
		MouseMovedEvent(float x,float y)
			:m_X(x),m_Y(y){}

		int GetCategory()const override
		{
			return (EventCategoryMouse | EventCategoryInput);
		}
		EventType GetType()const override
		{
			return EventType::MouseMoved;
		}


		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent(x,y):" << "(" << m_X << "," << m_Y << ")";
			return ss.str();
		}

		float GetPosX()const { return m_X; }
		float GetPos¦´()const { return m_Y; }


	private:
		float m_Y;
		float m_X;
	};


	class MouseScrolledEvent:public Event
	{
	public:
		MouseScrolledEvent(float offsetx, float offsety)
			:m_OffsetX(offsetx), m_OffsetY(offsety){}
		
		int GetCategory()const override
		{
			return (EventCategoryMouse | EventCategoryInput);
		}
		EventType GetType()const override
		{
			return GetEventType();
		}

		static EventType GetEventType() { return EventType::MouseScrolled; }


		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent(x,y):" << "(" << m_OffsetX << "," << m_OffsetY << ")";
			return ss.str();
		}

		float GetOffsetX()const { return m_OffsetX; }
		float GetOffsetY()const { return m_OffsetY; }
	private:
		float m_OffsetX;
		float m_OffsetY;
	};



	class MouseButtonEvent :public Event
	{
	public:
		MouseButtonEvent(int button)
			:m_Button(button){}
		virtual ~MouseButtonEvent() = default;

		int GetCategory()const override
		{
			return (EventCategoryMouseButton | EventCategoryMouse | EventCategoryInput);
		}

		int GetButton()const { return m_Button; }
	private:
		int m_Button;
	};


	class MouseButtonPressedEvent :public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			:MouseButtonEvent(button){}
		

		EventType GetType()const override
		{
			return GetEventType();
		}

		static EventType GetEventType() { return EventType::MouseButtonPressed; }


		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent:" << MouseButtonEvent::GetButton();
			return ss.str();
		}

	};


	class MouseButtonReleaseddEvent :public MouseButtonEvent
	{
	public:
		MouseButtonReleaseddEvent(int button)
			:MouseButtonEvent(button) {}


		EventType GetType()const override
		{
			return GetEventType();
		}

		static EventType GetEventType() { return EventType::MouseButtonReleased; }


		std::string ToString()const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent:" << MouseButtonEvent::GetButton();
			return ss.str();
		}

	};


}