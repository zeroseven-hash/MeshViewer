#include"pch.h"
#include"Window.h"


//currently only support windows platform
#ifdef PLATFOR_WINDOWS
#include<Platform/Windows/WindowsWindow.h>
#endif // PLATFOR_WINDOWS

namespace MV
{
	Unique<Window> Window::Create(const WindowProps& props)
	{
#ifdef PLATFOR_WINDOWS
		return std::make_unique<WindowsWindow>(props);
#endif

	}
}