#pragma once

#include"glm/glm.hpp"

namespace MV
{
	class DrawablePolygonMesh;
	class DrawablePolyhedralMesh;
	struct Face;
	struct Cell;
	class MeshFilter
	{
	public:
		MeshFilter() = default;
		~MeshFilter() = default;

		
		float GetLimitX()const { return m_X; }
		float GetLimitY()const { return m_Y; }
		float GetLimitZ()const { return m_Z; }
		float GetLimitJacobian()const { return m_Jacobian; }


		void SetLimitX(float x) { m_X = x; }
		void SetLimitY(float y) { m_Y = y; }
		void SetLimitZ(float z) { m_Z = z; }
		void SetLimitJacobian(float jac) { m_Jacobian = jac; }


		bool IsFilter(const DrawablePolygonMesh* const mesh, const Face& cell)const;
		bool IsFilter(const DrawablePolyhedralMesh* const mesh, const Cell& cell)const;
		bool IsFilter(const glm::vec3& minBound, const glm::vec3& maxBound, const glm::vec3& pos)const;
	private:
		//x y z bound,range:0.0-1.0f
		float m_X = 1.0f, m_Y = 1.0f, m_Z = 1.0f;
		//Jacobian bound ,range:0.0-1.0f
		float m_Jacobian = 1.0f;

		
	};
}