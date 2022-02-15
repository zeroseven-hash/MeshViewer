#pragma once
#include"DrawableMesh.h"

#include"glm/glm.hpp"
namespace MV
{
	
	//vary basic mesh
	struct Vert
	{
		uint32_t Id;
		glm::vec3 Position;
		glm::vec3 Normal={0.0f,0.0f,0.0f};
		Vert(uint32_t id,float x, float y, float z)
			:Id(id),Position(x, y, z){};
	};

	struct Face
	{
		uint32_t Id;
		std::vector<uint32_t> Indices;
		glm::vec3 Normal;
		glm::vec3 Color;

		//Flag
		
		bool IsFilter=false;
		bool IsBoundary=true; //Only for hex and tet


		Face() = default;
		//Triangle face
		Face(uint32_t id,uint32_t v0, uint32_t v1, uint32_t v2,const glm::vec3& color)
			:Id(id),Color(color)
		{
			Indices = { v0,v1,v2 };
		}


		//Quad face
		Face(uint32_t id,uint32_t v0, uint32_t v1, uint32_t v2, uint32_t v3,const glm::vec3& color)
			:Id(id),Color(color)
		{
			Indices = { v0,v1,v2,v3 };
		}

		bool operator==(const Face& other) const 
		{
			
			/*if (this->Indices.size() != other.Indices.size())
				return false;*/
			std::vector<uint32_t> i1;
			std::vector<uint32_t> i2;
			i1.assign(this->Indices.begin(), this->Indices.end());
			i2.assign(other.Indices.begin(), other.Indices.end());
			std::sort(i1.begin(), i1.end(), [](uint32_t a, uint32_t b) {return a < b; });
			std::sort(i2.begin(), i2.end(),[](uint32_t a, uint32_t b) {return a < b; });
			for (int i = 0; i < i1.size(); i++)
			{
				if (i1[i] != i2[i])
					return false;
			}
			return true;
		}
		

	};
	struct FaceHashFunc
	{

		size_t operator() (const Face& face) const
		{
			int n = (int)face.Indices.size();
			if (n == 3)
			{
				return std::hash<uint32_t>()(face.Indices[0]) ^ std::hash<uint32_t>()(face.Indices[1])^std::hash<uint32_t>()(face.Indices[2]);
			}
			else
			{
				return std::hash<uint32_t>()(face.Indices[0]) ^ std::hash<uint32_t>()(face.Indices[1])
					^ std::hash<uint32_t>()(face.Indices[2]) ^ std::hash<uint32_t>()(face.Indices[3]);
			}
		}
	};


	class DrawablePolygonMesh :public DrawableMesh
	{
	public:
		DrawablePolygonMesh() = default;
		DrawablePolygonMesh(const VERTEXLIST& vertices, const CELLLIST& cells_or_faces,uint32_t id=0);
		~DrawablePolygonMesh()=default;

		virtual void LoadData(
			const VERTEXLIST&		vertices,
			const CELLLIST&		cells_or_faces
		)override;
		virtual void UpdateJacobian()override;
		virtual void UpdateGL()override;
		virtual void UpdateBoundingBox()override;
		virtual void UpdateFilter(float x, float y, float z, float jacobian)override;
		virtual void Normalized(glm::mat4& transform = glm::mat4(1.0f))override;

		virtual void ResetColor(const glm::vec3& color)override;
		virtual void SetCellColor(uint32_t id, const glm::vec3& color)override;
		
		
		virtual void SavePl2(const char* filepath, const glm::mat4& transform = glm::mat4(1.0f))override;


		virtual size_t GetVertNum()override { return m_Vertices.size(); }
		virtual size_t GetCellNum()override { return m_Faces.size(); }

		const std::vector<Face>& GetFaces() const { return m_Faces; }
		const std::vector<Vert>& GetVertices() const { return m_Vertices; }
		const Face& GetFace(uint32_t id)const { return m_Faces[id]; }
		const Vert& GetVert(uint32_t id)const { return m_Vertices[id]; }




		void SetVert(uint32_t id, const Vert& new_vert) { m_Vertices[id] = new_vert; m_Vertices[id].Id = id; }

	private:
		void UpdateNormalPerFace();
	private:
		std::vector<Face> m_Faces;
		std::vector<Vert> m_Vertices;
	};
}