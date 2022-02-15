#pragma once
#include<string>
#include<CoreModel/Macro.h>
namespace MV
{
	enum class EventType
	{
		WindowClosed,WindowResized,WindowDrop,
		KeyPressed,KeyReleased,KeyTyped,
		MouseButtonPressed,MouseButtonReleased,MouseMoved,MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryAppication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

	class Event
	{
	public:
		virtual ~Event() = default;
		virtual int GetCategory()const = 0;
		virtual EventType GetType()const = 0;
		virtual std::string ToString()const = 0;

		inline bool GetHandled()const { return m_Handled; }
		inline void SetHandled(bool handled) { m_Handled = handled; }
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategory() & category;
		}


	private:
		bool m_Handled = false;
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& e):
			m_Event(e){}

		template<typename T>
		using EventFn = std::function<bool(T&)>;

		template<typename T>
		void Dispatch(const EventFn<T>& eventFn)
		{
			if (m_Event.GetType() == T::GetEventType())
			{
				bool handled= eventFn(*(T*)&m_Event);
				m_Event.SetHandled(handled);
			}
		}
	private:
		Event& m_Event;
	};
}