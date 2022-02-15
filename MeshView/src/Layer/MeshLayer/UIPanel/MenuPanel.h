#pragma once
#include<CoreModel/Layer.h>


#include"../ToolPanel/ColorController.h"
namespace MV
{
	class Entity;
	class MenuPanel
	{
	public:
		MenuPanel() = default;
		MenuPanel(Layer* context) :m_Context(context) {}

		void SetContext(Layer* context) { m_Context = context; }

		void OnImGuiRender();
		void OpenFile(const std::string& filepath);
		void SaveFile(const Entity& entity,const std::string& filepath);
	private:
		Layer* m_Context;

		ColorController m_ColorController;
	};



}