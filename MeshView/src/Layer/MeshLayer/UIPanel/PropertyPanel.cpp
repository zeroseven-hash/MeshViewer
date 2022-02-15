#include"pch.h"
#include "PropertyPanel.h"

#include"DataStructure/DrawableMesh.h"
#include"DataStructure/DrawablePoint.h"
#include"DataStructure/DrawableLine.h"
#include<imgui.h>
#include<glm/gtc/type_ptr.hpp>
namespace MV
{
	static std::string MeshtypeToString(MeshType type)
	{
		switch (type)
		{
		case MeshType::HEX:			return "Hexahedron";
		case MeshType::QUADMESH:	return "Quadrilateral";
		case MeshType::TET:			return "Tetrahedron";
		case MeshType::TRIMESH:		return "Triangle";
		default:IGAME_ASSERT(false, " ");
		}
	}
	static void DrawMeshProperty(Ref<DrawableMesh>& meshPtr)
	{
		static glm::vec3 meshColor = meshPtr->GetMeshSoildColor();
		static glm::vec3 lineColor = meshPtr->GetMeshLineColor();
		static float alpha = meshPtr->GetAlpha();
		MeshType meshtype = meshPtr->GetMeshType();

		ImGuiIO& io = ImGui::GetIO();
		auto& boldFont = io.Fonts->Fonts[0];
		ImGui::PushFont(boldFont);
		ImGui::Text("Statistics");
		ImGui::PopFont();
		ImGui::Separator();
		ImGui::Text("MeshType: %s", MeshtypeToString(meshtype).c_str());
		ImGui::Text("Number of Vertices: %d", meshPtr->GetVertNum());
		ImGui::Text("Number of Cells:    %d", meshPtr->GetCellNum());
		if (ImGui::ColorEdit3("Mesh Color", glm::value_ptr(meshColor)))
		{
			meshPtr->SetMeshSoildColor(meshColor);
		}
		if (ImGui::ColorEdit3("Edge Color", glm::value_ptr(lineColor)))
		{
			meshPtr->SetMeshLineColor(lineColor);
		}
		if (ImGui::SliderFloat("Alpha", &alpha, 0.0f, 1.0f))
		{
			meshPtr->SetAlpha(alpha);
		}
		ImGui::SameLine();
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("when alpha is not 1,Renderer will be wrong, Should be fix!!!");
		}

		
		ImGui::PushFont(boldFont);
		ImGui::Text("Quality");
		ImGui::PopFont();
		ImGui::Separator();
		const auto& jacobian = meshPtr->GetJacobian();

		auto& boldItalicFont = io.Fonts->Fonts[1];
		boldItalicFont->FontSize=18.0f;
		ImGui::PushFont(boldItalicFont);
		ImGui::Text("MinJ:%.3f",jacobian.MinJ);ImGui::SameLine();
		ImGui::Text("MaxJ:%.3f", jacobian.MaxJ);ImGui::SameLine();
		ImGui::Text("AvgJ:%.3f", jacobian.AvgJ);ImGui::SameLine();
		ImGui::Text("Deviation:%.3f", jacobian.DeviationJ);
		ImGui::PopFont();
	}


	static void DrawLineProperty(Ref<DrawableLine>& linePtr)
	{
		static glm::vec4 lineColor = { linePtr->GetLineColor(),linePtr->GetAlpha()};
		ImGui::Text("Statistics:");
		ImGui::Text("Type: %s", "Line");
		ImGui::Text("Number of Line: %d", linePtr->GetLineNum());
		ImGuiColorEditFlags flag = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf;
		if (ImGui::ColorEdit4("Line Color", glm::value_ptr(lineColor), flag))
		{
			linePtr->SetLineColor(glm::vec3(lineColor[0], lineColor[1],lineColor[2]));
			linePtr->SetAlpha(lineColor[3]);
		}
		static float lineWidth = linePtr->GetWidth();
		if (ImGui::SliderFloat("Line Width", &lineWidth,0.0f, 10.0f))
		{
			linePtr->SetWidth(lineWidth);
		}
	}


	static void DrawPointProperty(Ref<DrawablePoint>& pointPtr)
	{
		static glm::vec4 pointColor = { pointPtr->GetPointColor(),pointPtr->GetAlpha() };
		ImGui::Text("Statistics:");
		ImGui::Text("Type: %s", "Point");
		ImGui::Text("Number of Point: %d", pointPtr->GetPointNum());
		ImGuiColorEditFlags flag = ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf;
		if (ImGui::ColorEdit4("Line Color", glm::value_ptr(pointColor), flag))
		{
			pointPtr->SetPointColor(glm::vec3(pointColor[0], pointColor[1], pointColor[2]));
			pointPtr->SetAlpha(pointColor[3]);
		}
		static float pointSize = pointPtr->GetSize();
		if (ImGui::SliderFloat("Point Size", &pointSize, 0.0f, 20.0f))
		{
			pointPtr->SetSize(pointSize);
		}
	}
	void PropertyPanel::OnImGuiRender()
	{
		ImGui::Begin("Properties");
		if (m_Context->GetSelectedContext())
		{
			Entity entity = m_Context->GetSelectedContext();
			auto drawObjPtr = entity.GetComponent<DrawableComponent>().Object;

			switch (drawObjPtr->GetObjectType())
			{
			case ObjectType::MESH:
				DrawMeshProperty(std::static_pointer_cast<DrawableMesh>(drawObjPtr));
				break;
			case ObjectType::LINE:
				DrawLineProperty(std::static_pointer_cast<DrawableLine>(drawObjPtr));
				break;
			case ObjectType::POINT:
				DrawPointProperty(std::static_pointer_cast<DrawablePoint>(drawObjPtr));
				break;
			default:
				break;
			}

		}

		ImGui::End();
	}

}
