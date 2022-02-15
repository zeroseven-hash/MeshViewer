#include"pch.h"
#include"MeshFilter.h"

#include"DrawablePolygonMesh.h"
#include"DrawablePolyhedralMesh.h"
namespace MV
{

	bool MeshFilter::IsFilter(const DrawablePolygonMesh* const mesh, const Face& cell)const
	{
		//position check
		for (auto& vid : cell.Indices)
		{
			
			const auto& pos = mesh->GetVert(vid).Position;
			if (IsFilter(mesh->GetMinBound(), mesh->GetMaxBound(), pos)) return true;
		}
		//jacobian check
		if (mesh->GetJacobian().CellJacobian[cell.Id] > m_Jacobian) return true;
		
		return false;
	}

	bool MeshFilter::IsFilter(const DrawablePolyhedralMesh*const mesh, const Cell& cell)const
	{
		//position check
		for (auto& vid : cell.Vs)
		{
			const auto& pos = mesh->GetVert(vid).Position;
			if (IsFilter(mesh->GetMinBound(), mesh->GetMaxBound(), pos)) return true;
		}

		//jacobian check
		if (mesh->GetJacobian().CellJacobian[cell.Id] > m_Jacobian) return true;


		return false;
	}

	bool MeshFilter::IsFilter(const glm::vec3& minBound,const glm::vec3& maxBound, const glm::vec3& pos)const
	{
		glm::vec3 size = (maxBound - minBound);
		auto float_equal = [](float x,float y){return std::abs(x - y)<FLT_EPSILON;};
		float limits[3] = { m_X,m_Y,m_Z };
		if (float_equal(m_X, 1.0f) && float_equal(m_Y, 1.0f) && float_equal(m_Z, 1.0f))
			return false;
		if (float_equal(m_X, 0.0f) && float_equal(m_Y, 0.0f) && float_equal(m_Z, 0.0f))
			return true;
		for (int i = 0; i < 3; i++)
		{
			if (pos[i] > minBound[i] + size[i]*limits[i]) 
				return true;
		}
		return false;
	}

	
}