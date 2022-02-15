#pragma once
#include<CoreModel/Log.h>
#include<CoreModel/Layer.h>
#include<CoreModel/Application.h>
#include<Event/KeyEvent.h>
//Renderer Part
#include<Renderer/VertexArrayBuffer.h>
#include<Renderer/Shader.h>
#include<Renderer/FrameBuffer.h>
#include<Renderer/Camera.h>

//panel
#include"UIPanel/LogPanel.h"
#include"UIPanel/MenuPanel.h"
#include"UIPanel/ScenePanel.h"
#include"UIPanel/PropertyPanel.h"
#include"UIPanel/RenderPanel.h"
#include"UIPanel/FilterPanel.h"
#include<imgui.h>

#include"DataStructure/DrawableMesh.h"
#include"DataStructure/DrawableArrow.h"
namespace MV
{
	struct CoordinateAxis
	{
		Ref<DrawableArrow> X;
		Ref<DrawableArrow> Y;
		Ref<DrawableArrow> Z;
		CoordinateAxis(const glm::vec3& renderPos)
		{
			X = std::make_shared<DrawableArrow>(renderPos, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.1f, 0.006f);
			Y = std::make_shared<DrawableArrow>(renderPos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 0.006f);
			Z = std::make_shared<DrawableArrow>(renderPos, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.1f, 0.006f);
		}
		CoordinateAxis(glm::vec3&& renderPos)
		{
			X = std::make_shared<DrawableArrow>(renderPos, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.1f, 0.006f);
			Y = std::make_shared<DrawableArrow>(renderPos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.1f, 0.006f);
			Z = std::make_shared<DrawableArrow>(renderPos, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), 0.1f, 0.006f);
		}
		void OnUpdate(const glm::mat4& rotation, Ref<Shader>& shader);
	};

	class MeshLayer :public Layer
	{
	public:
		friend class ScenePanel;
	public:
		MeshLayer(const std::string& name);

		void OnAttach()override;

		void OnUpdate(TimeStep ts)override;
		void OnImGuiRender()override;
		void OnEvent(Event& e)override;

		void OnEntityConstruction(Entity& entity) override;
		Entity& GetSelectedContext() override { return m_SelectEntity; };

		void OnViewportResize(uint32_t newWidth, uint32_t newHeight);
		

		bool OnWindowDropEvent(WindowDropEvent& e);
		bool OnKeyPressedEvent(KeyPressedEvent& e);
		bool OnMouseButtonPreessEvent(MouseButtonPressedEvent& e);
	private:
		void CameraFocus(float distance, const glm::vec3& focalPoint);
		void DrawButtons();
		void DrawImGuizmo();
	private:
		//Render part
		Ref<Shader> m_Lightshader;
		Ref<Shader> m_MeshLineShader;
		Ref<Shader> m_StencilShader;
		Ref<Shader> m_ArrowShader;
		Camera m_Camera;

		bool m_ViewportHovered;
		bool m_ViewportFocusedOrHovered;
		//ViewportControll 
		uint32_t m_ViewportWidth, m_ViewportHeight;
		glm::vec2 m_ViewportBounds[2];

		std::function<void()> OnViewportResizeFn = nullptr;

		Entity m_SelectEntity;
		
		
		//panel
		LogPanel		m_LogPanel;
		MenuPanel		m_MenuPanel;
		ScenePanel		m_ScenePanel;
		PropertyPanel	m_PropertyPanel;
		RenderPanel		m_RendererPanel;
		FilterPanel		m_FilterPanel;
 		//Texture
		Texture m_FocusTexture;
		Texture m_FilterTexture;
		
		
		
	

		Ref<FrameBuffer> m_Gbuffer;
		Ref<FrameBuffer> m_SsaoFBO;
		Ref<FrameBuffer> m_SsaoBlurFBO;
		Ref<FrameBuffer> m_LightFBO;
		
		Ref<CoordinateAxis> m_CoordinateAxis;
		Ref<DrawableArrow> arrow;
		uint32_t m_SampleCount=2;

		int m_GizmoMode = -1;
		bool m_GizmoUsing=false;
	};

}
