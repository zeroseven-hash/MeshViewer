#pragma once

namespace MV
{
	class Entity;
	
	class ColorController
	{
		friend class MenuPanel;
	public:

		ColorController() = default;
		~ColorController() = default;
		void OnImGuiRender(Entity selectedEntity);

	private:
		bool m_Open = false;
	};
}