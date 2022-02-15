#pragma once
#include"CoreModel/Layer.h"


namespace MV
{
	class FilterPanel
	{
	public:
		FilterPanel() = default;
		~FilterPanel() = default;

		void SetContext(Layer* context) { m_Context = context; }
		void OnImGuiRender();
	private:
		Layer* m_Context;
	};
}