#pragma once
#include<CoreModel/Layer.h>

namespace MV
{
	class ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer(const std::string& name = "ImGuiLayer");
		~ImGuiLayer();


		void OnAttach()override;
		void OnUpdate(TimeStep ts)override;
		void OnImGuiRender()override;
		void OnEvent(Event& e)override;

		void Begin();
		void End();
	

		void BlockEvent(bool block);
	private:
		void SetImGuiStyle();
		void SetIniSetting();
	private:
		bool m_BlockEventFlag = false;
		
	};
}