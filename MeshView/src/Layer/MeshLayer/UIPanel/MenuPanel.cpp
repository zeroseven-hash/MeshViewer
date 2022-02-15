#include "pch.h"
#include "MenuPanel.h"

#include<imgui.h>
#include<imgui_internal.h>
#include<CoreModel/FileDialogs.h>
#include<CoreModel/Application.h>
#include<DataStructure/MeshFileLoader.h>
#include<DataStructure/DrawableMesh.h>
namespace MV
{
	void MenuPanel::OnImGuiRender()
	{
		bool p_open = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &p_open, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			dockspace_flags |= ImGuiDockNodeFlags_NoWindowMenuButton;
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{

				if (ImGui::MenuItem("Open...", "Ctrl+O"))
				{
					std::string filepath = FileDialogs::OpenFile(
						"SufaceMesh File(*.obj,*.off,*.vtk,*.Pl2,*ps,*ls)\0*.vtk;*.obj;*.off;*.pl2;\0VolumeMesh File(*.vtk)\0*.vtk\0Custom File(*.ls,*ps)\0*.ls;*.ps\0\0"
					);
					OpenFile(filepath);
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
				{
					Entity entity = m_Context->GetSelectedContext();
					if (entity)
					{
						if (entity.GetComponent<DrawableComponent>().Object->GetObjectType() == ObjectType::MESH)
						{
							auto mesh_ptr = std::static_pointer_cast<DrawableMesh>(entity.GetComponent<DrawableComponent>().Object);
							std::string filter;
							switch (mesh_ptr->GetMeshType())
							{
							case MeshType::HEX:
								filter = "Vtk File(*.vtk)\0*.vtk\0Ovm File(*.ovm)\0*.ovm\0Mesh File(*.mesh)\0*.mesh\0\0"; break;
							case MeshType::TET:
								filter = "Vtk File(*.vkt)\0*.vtk\0\0"; break;
							case MeshType::TRIMESH:
								filter = "Vtk File(*.vkt)\0*.vtk\0Off File(*.off)\0*off\0Obj File(*.obj)\0*.obj\0\0"; break;
							case MeshType::QUADMESH:
								filter = "Vtk File(*.vkt)\0*.vtk\0Off File(*.off)\0*off\0Obj File(*.obj)\0*.obj\0\0"; break;
							}
							std::string filepath = FileDialogs::SaveFile(filter.c_str());
							SaveFile(entity, filepath);
						}
						else
							MESH_ERROR("Currently only support save mesh");
						
					}
					else
						MESH_ERROR("Please select a mesh before saving!");
					
				}


				if (ImGui::MenuItem("Exit")) Application::Get().Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("ColorController", nullptr, &m_ColorController.m_Open);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		ImGui::End();

		if(m_ColorController.m_Open)
			m_ColorController.OnImGuiRender(m_Context->GetSelectedContext());
	}

	void MenuPanel::OpenFile(const std::string& filepath)
	{
		if (filepath != "")
		{
			auto entity = MeshFileLoader::LoadFile(filepath);
			m_Context->OnEntityConstruction(entity);

		}
	}
	void MenuPanel::SaveFile(const Entity& meshEntity, const std::string& filepath)
	{
		if (filepath != "")
		{
			MeshFileLoader::SaveFile(meshEntity, filepath);
		}
	}
}

