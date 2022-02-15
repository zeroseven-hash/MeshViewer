#pragma once
#include<CoreModel/Layer.h>
namespace MV
{
	class PropertyPanel
	{
	public:

		PropertyPanel() = default;
		PropertyPanel(Layer* context)
			:m_Context(context){}

		void SetContext(Layer* context) { m_Context = context; }

		void OnImGuiRender();
	private:
		Layer* m_Context;
	};
}