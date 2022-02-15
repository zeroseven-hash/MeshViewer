#include"pch.h"
#include"Application.h"
#include"Input.h"
#include"TimeStep.h"
#include"Layer/MeshLayer/MeshLayer.h"
#include"Renderer/Renderer.h"



namespace MV
{
	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
	{
		IGAME_ASSERT(s_Instance == nullptr, "Application already exists!");
		s_Instance = this;
		
		

		m_Window = Window::Create(WindowProps(name));
		m_Window->SetEventCallbackFn(std::bind(&Application::OnEvent,this,std::placeholders::_1));
		

		//shoule init after window created
		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
		PushLayer(new MeshLayer("meshLayer"));

	}

	

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowClosedEvent>(std::bind(&Application::OnWindowClose, this,std::placeholders::_1));

		
		auto head = m_LayerList.GetHeadPointer();
		while (head!= nullptr)
		{
			if (e.GetHandled())
				break;
			head->Layer->OnEvent(e);
			head = head->Next;
		}
	}

	void Application::Run()
	{
		while (m_Runing)
		{
			float time = (float)ImGui::GetTime();
			TimeStep ts(time - m_LastFrameTime);
			m_LastFrameTime = time;

			auto head = m_LayerList.GetHeadPointer();
			while (head!= nullptr)
			{
				head->Layer->OnUpdate(ts);
				head = head->Next;
			}


			{
				//Imgui render
				m_ImGuiLayer->Begin();
				auto head = m_LayerList.GetHeadPointer();
				while (head != nullptr)
				{
					head->Layer->OnImGuiRender();
					head = head->Next;
				}
				m_ImGuiLayer->End();
			}


			m_Window->OnUpdate();
		}
	}

	void Application::Close()
	{
		m_Runing = false;
	}

	

	void Application::PushLayer(Layer* layer)
	{
		m_LayerList.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerList.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::PopLayer(const std::string& layerName)
	{
		m_LayerList.PopLayer(layerName);
	}

	void Application::PopOverlay()
	{
		m_LayerList.PopOverlay();
	}

	Application* Application::Create(const std::string& name)
	{
		//只能存在一个application，单例

		new Application(name);
		return s_Instance;
	}

	bool Application::OnWindowClose(WindowClosedEvent& event)
	{
		m_Runing = false;
		return true;
	}



}