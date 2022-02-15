#include"pch.h"
#include "FilterPanel.h"

#include"DataStructure/DrawableMesh.h"
#include"imgui.h"

namespace MV
{
	void FilterPanel::OnImGuiRender()
	{
		auto entity = m_Context->GetSelectedContext();
		static float x=1.0f,y=1.0f,z=1.0f;
		static float jacobian = 1.0f;
		
		Ref<DrawableMesh> mesh_ptr = nullptr;
		if (entity)
		{	
			if (entity.GetComponent<DrawableComponent>().Object->GetObjectType() == ObjectType::MESH)
				mesh_ptr = std::static_pointer_cast<DrawableMesh>(entity.GetComponent<DrawableComponent>().Object);
		}

		if (mesh_ptr)
		{
			auto& filter = mesh_ptr->GetFilter();
			x = filter.GetLimitX();
			y = filter.GetLimitY();
			z = filter.GetLimitZ();
			jacobian = filter.GetLimitJacobian();
		}


		if (ImGui::SliderFloat("X", &x, 0.0f, 1.0f, "%.3f") && mesh_ptr) { mesh_ptr->UpdateFilter(x, y, z, jacobian); }
		if (ImGui::SliderFloat("Y", &y, 0.0f, 1.0f, "%.3f") && mesh_ptr) { mesh_ptr->UpdateFilter(x, y, z, jacobian); }
		if (ImGui::SliderFloat("Z", &z, 0.0f, 1.0f, "%.3f") && mesh_ptr) { mesh_ptr->UpdateFilter(x, y, z, jacobian); }
		if (ImGui::SliderFloat("Qulity", &jacobian, 0.0f, 1.0f, "%.3f") && mesh_ptr) { mesh_ptr->UpdateFilter(x, y, z, jacobian); }
		
	}
}