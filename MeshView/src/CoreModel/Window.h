#pragma once
#include"pch.h"
#include"Macro.h"
#include"Log.h"
#include"Event/Event.h"
namespace MV
{
	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		WindowProps(const std::string& title = "IGameView", uint32_t width = 1580, uint32_t height = 1000)
			:Title(title), Width(width), Height(height)
		{
		}
	};


	//abstruct class: support linux and windows easier
	class Window
	{
	public:

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetWidth()const = 0;
		virtual void SetEventCallbackFn(const std::function<void(Event&)>& callbackfn) = 0;

		virtual void* GetNativeWindow()const = 0;
		virtual void SetVsync(bool enable) = 0;
		virtual bool IsVsync()const = 0;

		static Unique<Window> Create(const WindowProps& props=WindowProps());
	};
}