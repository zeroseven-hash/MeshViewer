#pragma once

#include<CoreModel/Layer.h>
#include<Entity/Entity.h>
#include<Renderer/Texture.h>
namespace MV
{
	
	class ScenePanel
	{
	public:

	public:
		ScenePanel() 
			:m_OpenEyeTexture("./assets/images/eye_open.png"),
			m_CloseEyeTexture("./assets/images/eye_close.png"),
			m_IsPanelHovered(false) {}

		explicit ScenePanel(Layer* context)
			:m_Context(context),
			m_OpenEyeTexture("./assets/images/eye_open.png"),
			m_CloseEyeTexture("./assets/images/eye_close.png"),
			m_IsPanelHovered(false) {}
		void SetContext(Layer* context) { m_Context = context; }

		void OnImGuiRender();

		bool GetHoveredFlag() { return m_IsPanelHovered; }
	private:
		void DrawEntityNode(Entity& entity);
	private:
		Layer* m_Context;
		bool m_IsPanelHovered;
		Texture m_OpenEyeTexture;
		Texture m_CloseEyeTexture;
	};
}