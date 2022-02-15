#include"pch.h"
#include "ColorController.h"

#include<fstream>
#include<random>

#include<imgui.h>
#include<Entity/Entity.h>
#include<DataStructure/DrawablePolygonMesh.h>
#include<DataStructure/DrawablePolyhedralMesh.h>
#include<glm/gtc/type_ptr.hpp>
#include<CoreModel/FileDialogs.h>
namespace MV
{
	static void DrawMeshColorPanel(const Ref<DrawableMesh>& mesh)
	{
		static glm::vec3 meshColor = mesh->GetMeshSoildColor();
		static glm::vec3 meshLineColor = mesh->GetMeshLineColor();

		if(ImGui::ColorEdit3("MeshColor", glm::value_ptr(meshColor)))
			mesh->SetMeshSoildColor(meshColor);
		if(ImGui::ColorEdit3("MeshLineColor", glm::value_ptr(meshLineColor)))
			mesh->SetMeshLineColor(meshLineColor);



		static glm::vec3 cellColor = mesh->GetMeshSoildColor();
		ImGui::ColorEdit3("CellColor",glm::value_ptr(cellColor));
		ImGui::SameLine();
		static int id = 0;
		//temporary;
		static std::vector<uint32_t> cids;

		if (ImGui::Button("load tag cell ids"))
		{
			std::string filepath = FileDialogs::OpenFile("Tag cells File(*.txt)\0*.txt\0\0");
			if (!filepath.empty())
			{
				std::ifstream infile(filepath);
				uint32_t cid;
				while (infile >> cid)
				{
					cids.push_back(cid);
				}
				infile.close();
			}
		}

	
		

		if (ImGui::Button("Apply"))
		{
			for (auto& cid : cids)
			{
				mesh->SetCellColor(cid, cellColor);
			}
			cids.clear();
		}
		ImGui::SameLine();
		if (ImGui::Button("clear"))
		{
			mesh->ResetColor(mesh->GetMeshSoildColor());
			cids.clear();
		}


		if (ImGui::Button("load patches"))
		{
			std::string filepath = FileDialogs::OpenFile("patched File(*.txt)\0*.txt\0\0");
			if (!filepath.empty())
			{
				std::ifstream infile(filepath);
				uint32_t size;
				infile >> size;

				std::uniform_real_distribution<float> random_floats(0.0f, 1.0f);
				std::random_device generator;
				std::vector<glm::vec3> color;
				for (uint32_t i = 0; i < size; i++)
				{
					color.push_back(glm::vec3(random_floats(generator), random_floats(generator), random_floats(generator)));
				}
				int cellid; int patch_id;
				while (infile >> cellid >> patch_id)
				{
					mesh->SetCellColor(cellid, color[patch_id]);
				}
				infile.close();
			}
		}
		
	}
	void ColorController::OnImGuiRender(Entity selectedEntity)
	{
		
		
		if (ImGui::Begin("ColorController", &m_Open))
		{
			if (selectedEntity)
			{
				auto obj_ptr = selectedEntity.GetComponent<DrawableComponent>().Object;
				ObjectType objType = obj_ptr->GetObjectType();
				switch (objType)
				{
				case ObjectType::MESH:
					DrawMeshColorPanel(std::static_pointer_cast<DrawableMesh>(obj_ptr));
					break;
				case ObjectType::LINE:

					break;
				case ObjectType::POINT:
					break;
				default:
					break;
				}
			}
			
	
			
		}
		ImGui::End();

	}

}
