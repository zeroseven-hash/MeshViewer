#pragma once

#include<CoreModel/Window.h>

#include"WindowsDragDrop.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<functional>
namespace MV
{
	class WindowsWindow :public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;
		inline uint32_t GetHeight() const override { return m_Data.Height; }
		inline uint32_t GetWidth()const override { return m_Data.Width; }
		inline void SetEventCallbackFn(const std::function<void(Event&)>& callbackfn) override
		{
			m_Data.EventCallBackFn = callbackfn;
			m_DropTarget.SetCallBackFunc(callbackfn);
		}
		
		virtual void* GetNativeWindow()const override { return m_Window; };
		void SetVsync(bool enable) override;
		bool IsVsync()const override;

	private:
		void Init(const WindowProps& props);
	
	private:
		struct WindowData
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;
			std::function<void(Event&)> EventCallBackFn;
			bool Vsync;
		};

		WindowData m_Data;

		//for drop event
		WindwosDropTarget m_DropTarget;
		GLFWwindow* m_Window;
	};
}