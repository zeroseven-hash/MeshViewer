#pragma once
#include"DrawableMesh.h"
namespace MV
{
	//HEX
	/*
				   v
			4----------7
			|\     ^   |\
			| \    |   | \
			|  \   |   |  \
			|   5------+---6
			|   |  +-- |-- | -> u
			3---+---\--2   |
			 \  |    \  \  |
			  \ |     \  \ |
			   \|      w  \|
				0----------1
	*/

	
	struct Vert;			//equal to DrawblePolyhedralMesh
	struct Face;			//equal to DrawblePolyhedralMesh
	struct Cell
	{
		uint32_t Id;
		std::vector<Face> Faces;			//hex should have six faces, tet should have four faces
		std::vector<uint32_t> Vs;
		
		float ScaledJacobian;
		Cell() = default;
		Cell(uint32_t id, std::initializer_list<Face>& list)
			:Id(id), Faces(list) {};
		Cell(uint32_t id, const uint32_t* vs,int n,const glm::vec3& color);
		
	};
	

	class DrawablePolyhedralMesh:public DrawableMesh
	{
	public:
		DrawablePolyhedralMesh(const VERTEXLIST& vertices,const CELLLIST& cells,uint32_t id=0);
		~DrawablePolyhedralMesh() = default;



		virtual void LoadData(
			const VERTEXLIST&	vertices,
			const CELLLIST&		cells
		)override;
		virtual void UpdateJacobian()override;
		virtual void UpdateGL()override;
		virtual void UpdateBoundingBox()override;
		virtual void UpdateFilter(float x, float y, float z, float jacobian)override;
		virtual void Normalized(glm::mat4& transform = glm::mat4(1.0f))override;

		virtual void ResetColor(const glm::vec3& color)override;
		virtual void SetCellColor(uint32_t id, const glm::vec3& color)override;

		virtual void SaveVtk(const char* filepath, const glm::mat4& transform = glm::mat4(1.0f))override;

		virtual size_t GetVertNum()override { return m_Vertices.size(); }
		virtual size_t GetCellNum()override { return m_Cells.size(); }


		const std::vector<Cell>& GetFaces() const { return m_Cells; }
		const std::vector<Vert>& GetVertices() const { return m_Vertices; }
		const Cell& GetCell(uint32_t id)const { return m_Cells[id]; }
		const Vert& GetVert(uint32_t id)const { return m_Vertices[id]; }
	private:
		void UpdateNormalPerFace();
		void TagBoundaryFace();
	private:
		std::vector<Vert> m_Vertices;
		std::vector<Cell> m_Cells;

	};
}