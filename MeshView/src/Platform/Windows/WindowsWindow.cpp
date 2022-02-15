#include "pch.h"
#include "WindowsWindow.h"


//Event
#include<Event/WindowEvent.h>
#include<Event/MouseEvent.h>
#include<Event/KeyEvent.h>


#define GLFW_EXPOSE_NATIVE_WIN32
#include<GLFW/glfw3native.h>
namespace MV
{
	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		glfwTerminate();
	}

	void WindowsWindow::OnUpdate()
	{
		
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	

	void WindowsWindow::SetVsync(bool enable)
	{
		if (enable)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
		m_Data.Vsync = enable;
	}

	bool WindowsWindow::IsVsync()const
	{
		return m_Data.Vsync;
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Height = props.Height;
		m_Data.Width = props.Width;
		

		if (!glfwInit())
		{
			MESH_ERROR("glfw initializing failed!");
		}

		
	
		
		
		m_Window = glfwCreateWindow((int)m_Data.Width,(int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);

		HRESULT oleResult = OleInitialize(NULL);
		if (oleResult != S_OK)
		{
			MESH_ERROR("Couldn't initialize Ole");
		}
		RegisterDragDrop(glfwGetWin32Window(m_Window), (IDropTarget*)&m_DropTarget);
		
		
		if (!m_Window)
		{
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(m_Window);


		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			MESH_ERROR("Failed to initialize GLAD");
		}
		

		SetVsync(true);

		glfwSetWindowUserPointer(m_Window, &m_Data);


		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowResizedEvent e((uint32_t)width,(uint32_t)height);
			data.EventCallBackFn(e);
			
			#ifdef DEBUG
			MESH_TRACE("{0}", e.ToString());
			#endif
				
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowClosedEvent e;
			data.EventCallBackFn(e);

			#ifdef DEBUG
			MESH_TRACE("{0}", e.ToString());
			#endif

		});


		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int modes) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent e(key, 0);
					data.EventCallBackFn(e);

					#ifdef DEBUG
					MESH_TRACE("{0}", e.ToString());
					#endif
					break;
				}

				case GLFW_RELEASE:
				{
					KeyReleasedEvent e(key);
					data.EventCallBackFn(e);

					#ifdef DEBUG
					MESH_TRACE("{0}", e.ToString());
					#endif
					break;
				}
				
				case GLFW_REPEAT:
				{
					KeyPressedEvent e(key, 1);
					data.EventCallBackFn(e);

					#ifdef DEBUG	
					MESH_TRACE("{0}", e.ToString());
					#endif
					break;
				}
			}

		});



		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent e((float)xpos, (float)ypos);
			data.EventCallBackFn(e);
			#ifdef DEBUG	
			MESH_TRACE("{0}", e.ToString());
			#endif
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xoffset, double yoffset) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent e((float)xoffset, (float)yoffset);
			data.EventCallBackFn(e);
			#ifdef DEBUG	
			MESH_TRACE("{0}", e.ToString());
			#endif
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int modes) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent e(button);
					data.EventCallBackFn(e);
					#ifdef DEBUG
					MESH_TRACE("{0}", e.ToString());
					#endif
					break;
				}

				case GLFW_RELEASE:
				{
					MouseButtonReleaseddEvent e(button);
					data.EventCallBackFn(e);

					#ifdef DEBUG
					MESH_TRACE("{0}", e.ToString());
					#endif
					break;
				}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int character) {
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			//character is the acsii code
			KeyTypedEvent e(character);
			data.EventCallBackFn(e);

			#ifdef DEBUG
			MESH_TRACE("{0}", e.ToString());
			#endif
			});

	}

}
