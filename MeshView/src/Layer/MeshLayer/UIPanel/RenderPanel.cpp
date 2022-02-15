#include"pch.h"
#include "RenderPanel.h"

#include<Renderer/Renderer.h>
#include<Entity/Entity.h>
#include<imgui.h>
namespace MV
{
	static int GetRenderMode(Entity entity)
	{
		if (entity)
		{
			auto obj = entity.GetComponent<DrawableComponent>().Object;

			switch (obj->GetObjectType())
			{
			case ObjectType::MESH:return (int)std::static_pointer_cast<DrawableMesh>(obj)->GetRenderMode();
			case ObjectType::LINE: return 0;
			case ObjectType::POINT:return 0;
			}
		}
		else
			return 1;
	}
	static void DrawComponent(const char* label,const char*toolTip, const std::function<void()>& drawFunc)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto& boldFont = io.Fonts->Fonts[0];
		ImGui::PushFont(boldFont);
		ImGui::Text(label);
		ImGui::PopFont();
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip(toolTip);
		ImGui::Separator();
		
		
		
		drawFunc();
	}
	void MV::RenderPanel::OnImGuiRender()
	{
		ImGui::Begin("Renderer");
		//Representation part
		auto entity = m_Context->GetSelectedContext();
		
		
		//Render Mode;
		DrawComponent("Representation","Render Mode", [&] {
			std::vector<std::string> items;
			if (entity)
			{
				auto type = entity.GetComponent<DrawableComponent>().Object->GetObjectType();

				switch (type)
				{
				case ObjectType::MESH:items =  { "Surface", "SufaceWithEdges", "Wirefire", "Points","Volume" }; break;
				case ObjectType::LINE:items =  { "Wirefire"}; break;
				case ObjectType::POINT:items = { "Points"  }; break;
				}
			}
			else
			{
				items = { "Surface", "SufaceWithEdges", "Wirefire", "Points","Volume" };
			}
			
			static int item_current_idx =1; // Here we store our selection data as an index.
			if (entity)
				item_current_idx = GetRenderMode(entity);
			const char* combo_label = items[item_current_idx].c_str();  // Label to preview before opening the combo (technically it could be anything)

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.772f,0.828f,0.890f,1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.702f,0.758f,0.820f,1.0f });
			if (ImGui::BeginCombo("Render Mode", combo_label))
			{
				for (int n = 0; n < items.size(); n++)
				{
					const bool is_selected = (item_current_idx == n);
					if (ImGui::Selectable(items[n].c_str(), is_selected))
					{
						item_current_idx = n;
						if (entity)
						{
							auto obj = entity.GetComponent<DrawableComponent>().Object;
							if (obj->GetObjectType() == ObjectType::MESH)
								std::static_pointer_cast<DrawableMesh>(obj)->SetRenderMode((RenderMode)n);
						}
					}
					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::PopStyleColor(2);
			ImGui::SameLine();
			ImGui::TextDisabled("(?)");
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Super Sampling Anti-Aliasing");
		});
		



		DrawComponent("SSAA","Super Sampling Anti-Aliasing",[&] {

			//SSAA part
			const char* items[] = {"Sampling X1","Sampling X2", "Sampling X4", "Sampling X8"};
			static int item_current_idx = 1; // Here we store our selection data as an index.
			const char* combo_label = items[item_current_idx];  // Label to preview before opening the combo (technically it could be anything)
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.772f,0.828f,0.890f,1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.702f,0.758f,0.820f,1.0f });
			if (ImGui::BeginCombo("SSAA", combo_label))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					const bool is_selected = (item_current_idx == n);
					if (ImGui::Selectable(items[n], is_selected))
					{
						item_current_idx = n;
						m_SSAASample = (int)pow(2, n);
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::PopStyleColor(2);
		});
		

		DrawComponent("SSAO", "Scene Space Ambient occlusion", [&] {
			ImGui::Checkbox("SSAO", &m_SSAOFlag);
			if (m_SSAOFlag)
			{
				static int kernelSize = 24;
				static float radius = 0.5;
				static float bias = 0.025f;
				if (ImGui::SliderInt("SSAO KernelSize", &kernelSize, 0, 64))
				{
					Renderer::SetSSAOPatameter(kernelSize, radius, bias);
				}
				if (ImGui::SliderFloat("SSAO Radius", &radius, 0, 2.0f))
				{
					Renderer::Renderer::SetSSAOPatameter(kernelSize, radius, bias);
				}

				if (ImGui::SliderFloat("SSAO bias", &bias, 0.0f, 1.0f))
				{
					Renderer::Renderer::SetSSAOPatameter(kernelSize, radius, bias);
				}
			}
		});
		
		
		DrawComponent("Light", "##", [&] {
			static float ambient = 0.861f;
			static float diffuse = 0.718f;
			static float specular = 0.5f;
			m_LightShader->Bind();

			if (ImGui::SliderFloat("Ambient", &ambient,0.0f, 1.0f))
				m_LightShader->SetFloat("lightAmbient",ambient);
			if (ImGui::SliderFloat("Diffuse", &diffuse,0.0f, 1.0f))
				m_LightShader->SetFloat("lightdiffuse", diffuse);
			if (ImGui::SliderFloat("Specular", &specular,0.0f, 1.0f))
				m_LightShader->SetFloat("lightspec", specular);
		

		});
		ImGui::End();
	}

}