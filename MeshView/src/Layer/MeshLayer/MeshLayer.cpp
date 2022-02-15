#include "pch.h"
#include "MeshLayer.h"

#include"CoreModel/Input.h"
#include"Entity/EntityFactory.h"
#include"Renderer/Renderer.h"
#include"DataStructure/DrawableLine.h"
#include"DataStructure/DrawablePoint.h"
#include"Renderer/Texture.h"
#include"ImGuizmo.h"
#include"glm/gtc/type_ptr.hpp"
//temporary
#include<glad/glad.h>


namespace MV{

	void CoordinateAxis::OnUpdate(const glm::mat4& rotation, Ref<Shader>& shader)
	{

		Renderer::DrawArrow(shader, X, rotation, [&]() {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			});

		Renderer::DrawArrow(shader, Y, rotation, [&]() {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			});
		Renderer::DrawArrow(shader, Z, rotation, [&]() {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			});

	}


	MeshLayer::MeshLayer(const std::string& name)
		:Layer(name),
		m_ViewportWidth(1280),
		m_ViewportHeight(720),
		m_Camera(60.0f,1.778f,0.01f,1000.0f),
		m_FocusTexture("./assets/images/infographic.png"),
		m_FilterTexture("./assets/images/filter.png")
	{

	}

	void MeshLayer::OnAttach()
	{
		
		
		m_MeshLineShader = Shader::Create("./assets/shaders/meshshader_line.glsl");
		m_Lightshader = Shader::Create("./assets/shaders/light.glsl");
		m_StencilShader = Shader::Create("./assets/shaders/stencil_color.glsl");
		m_ArrowShader = Shader::Create("./assets/shaders/arrowshader.glsl");
		m_CoordinateAxis = std::make_shared<CoordinateAxis>(glm::vec3( -0.88f, -0.8f, 0.0f));
		
		//Init panel
		m_LogPanel.SetContext(this);
		m_MenuPanel.SetContext(this);
		m_ScenePanel.SetContext(this);
		m_PropertyPanel.SetContext(this);
		m_RendererPanel.SetContext(this);
		m_FilterPanel.SetContext(this);
		m_RendererPanel.SetLightShader(m_Lightshader);

		m_SampleCount = m_RendererPanel.GetSSAASample();

		m_Lightshader->Bind();
		m_Lightshader->SetInt1("gPosition", 0);
		m_Lightshader->SetInt1("gNormal", 1);
		m_Lightshader->SetInt1("gAlbedo", 2);
		m_Lightshader->SetInt1("ssao", 3);
		m_Lightshader->SetInt1("gIndex", 4);
		m_Lightshader->SetFloat3("lightColor", glm::vec3(0.95f, 0.95f, 0.95f));
		m_Lightshader->SetFloat3("material.ambient", glm::vec3(0.85f, 0.85f, 0.85f));
		m_Lightshader->SetFloat3("material.diffuse", glm::vec3(0.75f, 0.75f, 0.75f));
		m_Lightshader->SetFloat3("material.specular", glm::vec3(0.1f, 0.1f, 0.1f));
		m_Lightshader->SetFloat("material.shininess", 25.0f);
		m_Lightshader->SetFloat("lightAmbient", 0.861f);
		m_Lightshader->SetFloat("lightdiffuse", 0.718f);
		m_Lightshader->SetFloat("lightspec", 0.5f);

		FrameBufferAttachmentSpecification gBufferAttachmentsSpcs =
		{
			{TextureFormat::RGBA,TextureFilter::NEARST,TextureFilter::NEARST,TextureWrap::CLAMP_TO_EDGE,TextureWrap::CLAMP_TO_EDGE},//gPosition
			{TextureFormat::RGBA,TextureFilter::NEARST,TextureFilter::NEARST},//gNormal
			{TextureFormat::RGBA,TextureFilter::NEARST,TextureFilter::NEARST},//gAlbedo
			{TextureFormat::RED_INTERGER,TextureFilter::NEARST,TextureFilter::NEARST},//gIndex
			{TextureFormat::DEPTHANDSTENCIL}//depth

		};
		FrameBufferAttachmentSpecification ssaoAttachmentsSpcs =
		{
			{TextureFormat::RED,TextureFilter::NEARST,TextureFilter::NEARST},//gIndex
		};
		FrameBufferAttachmentSpecification lightAttachmentSpecs =
		{
			{TextureFormat::RGBA,TextureFilter::LINEAR,TextureFilter::LINEAR},
			{TextureFormat::DEPTHANDSTENCIL}
		};
		FrameBufferAttachmentSpecification ssaaAttachmentSpecs =
		{
			{TextureFormat::RGBA,TextureFilter::LINEAR,TextureFilter::LINEAR}
		};
		m_Gbuffer = FrameBuffer::Create(FrameBufferSpecification(m_ViewportWidth,m_ViewportHeight, gBufferAttachmentsSpcs,1));
		m_SsaoFBO= FrameBuffer::Create(FrameBufferSpecification(m_ViewportWidth, m_ViewportHeight, ssaoAttachmentsSpcs, 1));
		m_SsaoBlurFBO = FrameBuffer::Create(FrameBufferSpecification(m_ViewportWidth, m_ViewportHeight, ssaoAttachmentsSpcs, 1));

		m_LightFBO = FrameBuffer::Create(FrameBufferSpecification(m_ViewportWidth, m_ViewportHeight, lightAttachmentSpecs, 1));
		
	
	}

	void MeshLayer::OnUpdate(TimeStep ts)
	{
		if (OnViewportResizeFn)
			OnViewportResizeFn();

		if(m_ViewportFocusedOrHovered&&!m_GizmoUsing)
			m_Camera.OnUpdate(ts);

		Renderer::SetClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_LightFBO->Bind();
		
	
		Renderer::Clear();
	
		Renderer::Begin(m_Camera);
		
		auto view = EntityFactory::GetRegistry().view<TransformComponent, DrawableComponent>();
		uint32_t ssaaSample = m_RendererPanel.GetSSAASample();
		//Defered Rendering 用于渲染几何体
		//Genertate Gbuffer
		bool isGBuffer = false;
		m_Gbuffer->Bind();
		Renderer::Clear();
		m_Gbuffer->ClearAttachment(3, -1);

		//Enable Stencil
		glStencilMask(0x00);
		for (auto entity : view)
		{
			auto& [transform, drawobject] = EntityFactory::GetRegistry().get<TransformComponent, DrawableComponent>(entity);

			if (!drawobject.IsShow|| drawobject.Object->GetObjectType()!=ObjectType::MESH||Entity(entity)==m_SelectEntity) continue;
			isGBuffer = true;
			auto meshPtr = std::static_pointer_cast<DrawableMesh>(drawobject.Object);
			RenderMode mode = meshPtr->GetRenderMode();
			switch (mode)
			{
				case MV::RenderMode::SUFRACE:
				case MV::RenderMode::SUFACEWITHEDGES:
				case MV::RenderMode::VOLUME:
				{	
					
					Renderer::RenderMeshGbuffer(meshPtr, transform.Transform, [&](){
							glEnable(GL_POLYGON_OFFSET_FILL);
							glPolygonOffset(2.0f, 2.0f);
							glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					});
					break;
				}
				case MV::RenderMode::WIREFRAME:
				case MV::RenderMode::POINTS:break;
			}
		}
		
		//Stencil,Draw Select Entity
		if (m_SelectEntity)
		{

			auto& transform = m_SelectEntity.GetComponent<TransformComponent>().Transform;
			auto& drawobject = m_SelectEntity.GetComponent<DrawableComponent>();
			if (drawobject.IsShow &&drawobject.Object->GetObjectType() == ObjectType::MESH)
			{
				auto meshPtr = std::static_pointer_cast<DrawableMesh>(drawobject.Object);
				isGBuffer = true;
				RenderMode mode = meshPtr->GetRenderMode();
				switch (mode)
				{
				case MV::RenderMode::SUFRACE:
				case MV::RenderMode::SUFACEWITHEDGES:
				case MV::RenderMode::VOLUME:
				{
					glStencilFunc(GL_ALWAYS, 1, 0xFF);
					glStencilMask(0xFF);
					Renderer::RenderMeshGbuffer(meshPtr, transform, [&]() {
						glEnable(GL_POLYGON_OFFSET_FILL);
						glPolygonOffset(2.0f, 2.0f);
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						});
					break;
				}
				case MV::RenderMode::WIREFRAME:
				case MV::RenderMode::POINTS:break;
				}



				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);
				glDisable(GL_DEPTH_TEST);
				Renderer::DrawMesh(m_StencilShader, meshPtr, transform, [&]() {
					m_StencilShader->Bind();
					m_StencilShader->SetFloat("u_Scale",m_Camera.GetDistance());
					glEnable(GL_POLYGON_OFFSET_FILL);
					glPolygonOffset(2.0f, 2.0f);
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				});
				glStencilMask(0xFF);
				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				glEnable(GL_DEPTH_TEST);
			}
		}

		m_Gbuffer->UnBind();

		if (isGBuffer)
		{
			//Calulate SSAO
			uint32_t gPosition = m_Gbuffer->GetColorAttachment(0);
			uint32_t gNormal = m_Gbuffer->GetColorAttachment(1);
			uint32_t gAlbedo = m_Gbuffer->GetColorAttachment(2);
			uint32_t gIndex = m_Gbuffer->GetColorAttachment(3);
			uint32_t ssaoTexture = 0;
			//Calculate ssaoTexture 
			if (m_RendererPanel.GetSSAOFlag())
			{
				glDisable(GL_BLEND);
				Renderer::RenderSSAO(m_SsaoFBO, m_SsaoBlurFBO, gPosition, gNormal);
				ssaoTexture = m_SsaoBlurFBO->GetColorAttachment();
			}

			m_LightFBO->Bind();
			Renderer::Clear();
			m_Lightshader->Bind();
			m_Lightshader->SetInt1("IsSSAO", m_RendererPanel.GetSSAOFlag());
			glm::vec3 viewLightPos = m_Camera.GetViewMatrix() * glm::vec4(m_Camera.GetPosition() + m_Camera.GetUpDirection() * 4.5f, 1.0f);
			m_Lightshader->SetFloat3("lightPos", viewLightPos);

			//Defered shading
			Renderer::DefferedShading(m_Lightshader, gPosition, gNormal, gAlbedo, gIndex, ssaoTexture);
			glEnable(GL_BLEND);
		}


		//Forward shading 用于渲染点、线

		//设置深度缓冲
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_Gbuffer->GetRenderedId());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_LightFBO->GetRenderedId());
		glBlitFramebuffer(0, 0, m_ViewportWidth * ssaaSample, m_ViewportHeight * ssaaSample, 0, 0, m_ViewportWidth * ssaaSample, m_ViewportHeight * ssaaSample, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		for (auto entity : view)
		{
			auto& [transform, drawobject] = EntityFactory::GetRegistry().get<TransformComponent, DrawableComponent>(entity);
			if (!drawobject.IsShow) continue;
			switch (drawobject.Object->GetObjectType())
			{
				case ObjectType::MESH:
				{
					auto meshPtr = std::static_pointer_cast<DrawableMesh>(drawobject.Object);
					switch (meshPtr->GetRenderMode())
					{
						case RenderMode::SUFACEWITHEDGES:
						case RenderMode::WIREFRAME:
						{
							m_LightFBO->Bind();
							Renderer::DrawMesh(m_MeshLineShader, meshPtr, transform.Transform, [&]()
								{
									m_MeshLineShader->Bind();
									m_MeshLineShader->SetFloat4("u_Color", glm::vec4(meshPtr->GetMeshLineColor(), meshPtr->GetAlpha()));
									glEnable(GL_POLYGON_OFFSET_LINE);
									glPolygonOffset(0.0f, 0.0f);
									glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
									float size = (ssaaSample==1)?1.3f:2.5;
									glLineWidth(size);
								});
							break;
						}
						case RenderMode::POINTS:
						{
							Renderer::DrawPoint(m_MeshLineShader, meshPtr->GetVertexArrayBuffer(), transform.Transform, [&](){
								m_MeshLineShader->Bind();
								m_MeshLineShader->SetFloat4("u_Color", glm::vec4(meshPtr->GetMeshLineColor(), meshPtr->GetAlpha()));
								float size = (ssaaSample == 1) ? 1.0f : std::pow(2, ssaaSample);
								glPointSize(size*3.0f);
							});
							break;
						}
					}
					break;
				}
				case ObjectType::LINE:
				{
					auto linePtr = std::static_pointer_cast<DrawableLine>(drawobject.Object);
					Renderer::DrawLine(m_MeshLineShader, linePtr->GetVertexArrayBuffer(), transform.Transform, [&]() {
						m_MeshLineShader->Bind();
						m_MeshLineShader->SetFloat4("u_Color", glm::vec4(linePtr->GetLineColor(), linePtr->GetAlpha()));
						glLineWidth(linePtr->GetWidth());
					});
					break;

				}
				case ObjectType::POINT:
				{
					auto pointPtr = std::static_pointer_cast<DrawablePoint> (drawobject.Object);
					Renderer::DrawPoint(m_MeshLineShader, pointPtr->GetVertexArrayBuffer(), transform.Transform, [&]() {
						m_MeshLineShader->Bind();
						m_MeshLineShader->SetFloat4("u_Color", glm::vec4(pointPtr->GetPointColor(), pointPtr->GetAlpha()));
						glPointSize(pointPtr->GetSize());
						});
					break;
				}
				
			}

		}

		glm::mat4 rotation = glm::inverse(glm::toMat4(m_Camera.GetOrientation()));
		m_CoordinateAxis->OnUpdate(rotation, m_ArrowShader);
		Renderer::end();
		
		
		m_LightFBO->UnBind();


		
		
	}

	static void DrawTransparentText(const std::string& content, const ImVec2& right_corner_pos, const ImVec2& padding = {0.0f,0.0f})
	{
		ImGui::SetNextWindowBgAlpha(0.0f);
		static bool open = true;
		
		
		
		ImGui::SetNextWindowPos(ImVec2{ right_corner_pos.x - padding.x,right_corner_pos.y + padding.y }, ImGuiCond_Always, ImVec2{1,0});
		ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav| ImGuiWindowFlags_NoSavedSettings;

		if (ImGui::Begin("TransParent Text", &open, window_flags))
		{
			ImGui::Text(content.c_str());
		}
		ImGui::End();
	}

	void MeshLayer::OnImGuiRender()
	{
		m_MenuPanel.OnImGuiRender();
		m_LogPanel.OnImGuiRender();
		m_ScenePanel.OnImGuiRender();
		m_PropertyPanel.OnImGuiRender();
		m_RendererPanel.OnImGuiRender();

#if DEBUG

		
		ImGui::Begin("test");
		if (ImGui::Button("reset rotation"))
		{
			m_Camera.m_quaternion = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
		}
	
		
		ImGui::End();
		ImGui::ShowDemoWindow();
#endif
		
		//ViewPort panel
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f,0.0f });
		ImGuiWindowFlags flag = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse|ImGuiWindowFlags_NoMove;
		
		
		if (ImGui::Begin("ViewPort", nullptr, flag))
		{
			//calculate viewport bounds
			auto viewportOffset = ImGui::GetCursorPos();
			ImVec2 windowSize = { ImGui::GetWindowSize().x - viewportOffset.x, ImGui::GetWindowSize().y - viewportOffset.y };
			auto minBound = ImGui::GetWindowPos();
			minBound.x += viewportOffset.x;
			minBound.y += viewportOffset.y;
			ImVec2 maxBound = { minBound.x + windowSize.x,minBound.y + windowSize.y };
			m_ViewportBounds[0] = { minBound.x,minBound.y };
			m_ViewportBounds[1] = { maxBound.x,maxBound.y };


			std::string content = "FPS: " + std::to_string(ImGui::GetIO().Framerate).substr(0, 4);
			auto pos = ImGui::GetWindowPos();
			pos.x += ImGui::GetWindowWidth();
			pos.y += ImGui::GetCursorPosY();
			DrawTransparentText(content, pos, ImVec2{ 10.0f,10.0f });
			
			//
			ImVec2 viewportsize = ImGui::GetWindowSize();
			bool sampleFlag=m_SampleCount!= m_RendererPanel.GetSSAASample();
			if (sampleFlag)
				m_SampleCount = m_RendererPanel.GetSSAASample();

			if (m_ViewportWidth != (uint32_t)viewportsize.x || m_ViewportHeight != (uint32_t)viewportsize.y|| sampleFlag)
				OnViewportResizeFn = std::bind(&MeshLayer::OnViewportResize, this, (uint32_t)viewportsize.x, (uint32_t)viewportsize.y);
			else
				OnViewportResizeFn = nullptr;

			//m_LightFBO->GetColorAttachment()
			ImGui::Image((void*)m_LightFBO->GetColorAttachment(), ImVec2{(float)m_ViewportWidth,(float)m_ViewportHeight}, ImVec2{0,1}, ImVec2{1,0});
			
			
			//viewport buttons
			DrawButtons();
			DrawImGuizmo();
			m_ViewportFocusedOrHovered = (ImGui::IsWindowFocused()||ImGui::IsWindowHovered() && !ImGui::IsItemHovered());
			Application::Get().GetImGuiLayer()->BlockEvent(!m_ViewportFocusedOrHovered);
		
		}
		ImGui::End();
		ImGui::PopStyleVar();
		
	}

	void MeshLayer::OnEvent(Event& e)
	{
		EventDispatcher dispacher(e);
		dispacher.Dispatch<WindowDropEvent>(std::bind(&MeshLayer::OnWindowDropEvent, this, std::placeholders::_1));
		dispacher.Dispatch<MouseButtonPressedEvent>(std::bind(&MeshLayer::OnMouseButtonPreessEvent, this, std::placeholders::_1));
		dispacher.Dispatch<KeyPressedEvent>(std::bind(&MeshLayer::OnKeyPressedEvent, this, std::placeholders::_1));
		m_Camera.OnEvent(e);
	}



	void MeshLayer::OnEntityConstruction(Entity& entity)
	{
		if (entity)
		{
			auto& object_ptr = entity.GetComponent<DrawableComponent>().Object;
			auto& tc = entity.GetComponent<TransformComponent>().Transform;
			glm::vec3 pos = tc * glm::vec4(object_ptr->GetFocalPoint(), 1.0f);

			CameraFocus(2.3 * object_ptr->GetScaleFactor(), pos);
		}
	}


	void MeshLayer::OnViewportResize(uint32_t newWidth, uint32_t newHeight)
	{
		m_ViewportWidth = newWidth;
		m_ViewportHeight = newHeight;

		uint32_t m_SSAASample = m_RendererPanel.GetSSAASample();
		glViewport(0, 0, newWidth* m_SSAASample, newHeight* m_SSAASample);
		m_Gbuffer->Resize(newWidth * m_SSAASample, newHeight * m_SSAASample);
		m_SsaoFBO->Resize(newWidth * m_SSAASample, newHeight * m_SSAASample);
		m_SsaoBlurFBO->Resize(newWidth * m_SSAASample, newHeight * m_SSAASample);

		m_LightFBO->Resize(newWidth * m_SSAASample, newHeight * m_SSAASample);
		

		m_Camera.SetViewportSize((float)newWidth, (float)newHeight);
	}

	void MeshLayer::CameraFocus(float distance, const glm::vec3& focalPoint)
	{
		if (std::abs(distance - 0) < FLT_EPSILON)
			distance = 0.1f;
		m_Camera.SetDistance(distance);
		m_Camera.SetFocalPoint(focalPoint);
	}

	bool MeshLayer::OnWindowDropEvent(WindowDropEvent& e)
	{
		if (m_ScenePanel.GetHoveredFlag())
		{
			for (const auto& file : e.GetFiles())
			{
				m_MenuPanel.OpenFile(file);
			}
		}
		return true;
	}
	bool MeshLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
			return false;
		//Gizmos
		switch (e.GetKeyCode())
		{
		case IGAME_KEY_Q:m_GizmoMode = -1; break;
		case IGAME_KEY_W:m_GizmoMode = ImGuizmo::OPERATION::TRANSLATE; break;
		case IGAME_KEY_E:m_GizmoMode = ImGuizmo::OPERATION::ROTATE; ; break;
		case IGAME_KEY_R:m_GizmoMode = ImGuizmo::OPERATION::SCALE; ; break;
		default:break;
		}
	
		return true;
	}
	bool MeshLayer::OnMouseButtonPreessEvent(MouseButtonPressedEvent& e)
	{
		if (e.GetButton() != IGAME_MOUSE_BUTTON_LEFT) return true;

		if (Input::IsKeyPressed(IGAME_KEY_LEFT_ALT) || Input::IsKeyPressed(IGAME_KEY_RIGHT_ALT))
		{
			int sample = m_RendererPanel.GetSSAASample();
			auto [mx, my] = ImGui::GetMousePos();
			mx -= m_ViewportBounds[0].x;
			my -= m_ViewportBounds[0].y;
			mx *= sample;
			my *= sample;
			glm::vec2 viewportSize = (m_ViewportBounds[1] - m_ViewportBounds[0]);
			my = viewportSize.y * sample - my;
			int mouseX = (int)mx;
			int mouseY = (int)my;
			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x * sample && mouseY < (int)viewportSize.y * sample)
			{
				int pixelData = m_Gbuffer->ReadPixelI(3, mouseX, mouseY);
				m_SelectEntity = (pixelData == -1) ? Entity() : Entity((entt::entity)pixelData);
			}
		}
		return true;
	}
	void MeshLayer::DrawButtons()
	{
		ImVec2 itemPos = ImGui::GetWindowContentRegionMin();
		itemPos.x += 10.0f;
		itemPos.y += 10.0f;
		//filter button
		ImGui::SetItemAllowOverlap();
		ImGui::SetCursorPos(itemPos);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.867f,0.843f,0.843f,0.478f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.831f,0.816f,0.816f,0.906 });
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::BeginGroup();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 20.0f,20.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 8.0f);
		auto& style = ImGui::GetStyle();
		ImVec4 windwocolor = style.Colors[ImGuiCol_WindowBg];
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4{ windwocolor.x,windwocolor.y,windwocolor.z,0.3 });

		

		ImGui::ImageButton((void*)m_FilterTexture.GetRendererId(),
			ImVec2{ 20.0f,20.0f },
			ImVec2{ 0,1 },
			ImVec2{ 1,0 },
			0,
			ImVec4{ 1.0f,1.0f,1.0f,0.0f });
		
			
		if (ImGui::BeginPopupContextItem((const char*)0,ImGuiPopupFlags_MouseButtonLeft))
		{
			m_FilterPanel.OnImGuiRender();
			ImGui::EndPopup();
		}
		
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(3);

		

		//focues button
		ImGui::SameLine();
		if (ImGui::ImageButton((void*)m_FocusTexture.GetRendererId(),
			ImVec2{ 20.0f,20.0f },
			ImVec2{ 0,1 },
			ImVec2{ 1,0 },
			0,
			ImVec4{ 1.0f,1.0f,1.0f,0.0f }))
		{
			if (m_SelectEntity)
			{
				auto& object_ptr = m_SelectEntity.GetComponent<DrawableComponent>().Object;
				auto& tc = m_SelectEntity.GetComponent<TransformComponent>().Transform;
				glm::vec3 pos = tc * glm::vec4(object_ptr->GetFocalPoint(), 1.0f);
				CameraFocus(2.3 * object_ptr->GetScaleFactor(), pos);
			}
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Right-click to fucos on current selected mesh");


		ImGui::PopStyleColor(2);
		ImGui::PopStyleVar();
		ImGui::EndGroup();
	}

	void MeshLayer::DrawImGuizmo()
	{
		if (!m_SelectEntity||m_GizmoMode==-1) return;
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		float x = m_ViewportBounds[0].x;
		float y = m_ViewportBounds[0].y;
		glm::vec2 size = m_ViewportBounds[1] - m_ViewportBounds[0];
		ImGuizmo::SetRect(x,y,size.x,size.y);

		auto& tc = m_SelectEntity.GetComponent<TransformComponent>();
		auto pos = m_SelectEntity.GetComponent<DrawableComponent>().Object->GetFocalPoint();
		
		ImGuizmo::Manipulate(glm::value_ptr(m_Camera.GetViewMatrix()), glm::value_ptr(m_Camera.GetProjection()),
			(ImGuizmo::OPERATION)m_GizmoMode, ImGuizmo::MODE::LOCAL, glm::value_ptr(tc.Transform),nullptr, nullptr);
		m_GizmoUsing = ImGuizmo::IsUsing();
	}
}

