#pragma once

#include"Window.h"
#include"Event/Event.h"
#include"LayerList.h"
#include"Event/WindowEvent.h"
#include"Layer/ImGuiLayer/ImGuiLayer.h"


namespace MV
{

	


	class Application
	{
	public:
		//Application
		Application(const std::string& name);
		~Application() = default;

		void OnEvent(Event& e);
		void Run();
		void Close();

	
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
		void PopLayer(const std::string& layerName);
		void PopOverlay();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static inline  Application& Get() { return *s_Instance; }
		static Application* Create(const std::string& name);
		const Unique<Window>& GetWindow()const { return m_Window; }

		//Event
		bool OnWindowClose(WindowClosedEvent& event);


		
		
	private:

		bool m_Runing=true;

		Unique<Window> m_Window;

		float m_LastFrameTime=0.0f;
		ImGuiLayer* m_ImGuiLayer;
		LayerList m_LayerList;
		static Application* s_Instance;

	};

}