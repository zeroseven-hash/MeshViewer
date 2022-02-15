#include "pch.h"
#include "ScenePanel.h"

#include<imgui.h>
#include<imgui_internal.h>
#include"Entity/EntityFactory.h"
namespace MV
{



	void ScenePanel::OnImGuiRender()
	{
		
		if (ImGui::Begin("Scene"))
		{
		
			m_IsPanelHovered = ImGui::IsWindowHovered();
			EntityFactory::GetRegistry().each([&](auto entityId) {
				Entity entity(entityId);
				DrawEntityNode(entity);
				});
		}
		ImGui::End();
	
	}

	void ScenePanel::DrawEntityNode(Entity& entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		auto& show = entity.GetComponent<DrawableComponent>().IsShow;
		auto& m_SelectedContext = m_Context->GetSelectedContext();
		bool is_selected = (m_SelectedContext == entity);
		
		



		//eye button
		float button_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y + 3.0f;
		button_height *= 0.9;
		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		uint32_t id = (show)? m_OpenEyeTexture.GetRendererId() : m_CloseEyeTexture.GetRendererId();
		ImGui::PushID((uint32_t)entity);

		if (ImGui::ImageButton((void*)(intptr_t)id, ImVec2{button_height,button_height}, ImVec2{0,1}, ImVec2{1,0}, 0.0f, ImVec4(0, 0, 0, 0), ImVec4{1,1,1,1}))
		{
			id = (m_OpenEyeTexture.GetRendererId() == id) ? m_CloseEyeTexture.GetRendererId() : m_OpenEyeTexture.GetRendererId();
			show = !show;
		}
		ImGui::PopID();
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(3);
		const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
		ImGui::SameLine(0.0f, spacing);
		

		
		bool isSelect = (m_SelectedContext == entity) ? true : false;
		ImGui::PushID((uint32_t)entity);
		ImGui::Selectable(tag.c_str(), isSelect, ImGuiSelectableFlags_SpanAvailWidth);
		ImGui::PopID();

		if (ImGui::IsItemClicked())
		{
			m_SelectedContext = entity;
		}

		bool entityDeleted = false;

		
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;
			ImGui::EndPopup();
		}
		
		if (entityDeleted)
		{
			
			EntityFactory::DestroyEntity(entity);
			if (m_SelectedContext == entity)
				m_SelectedContext = {};
		}
	}

}
