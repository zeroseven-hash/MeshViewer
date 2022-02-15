#include"pch.h"

#include<fstream>

#include"DrawablePolygonMesh.h"
#include"glm/gtx/transform.hpp"


namespace MV
{
	static void SignedCornerAreas(double areas[4],glm::vec3 edges[4])
	{
		glm::vec3 cornerNormals[4];
		cornerNormals[0] = glm::cross(edges[3], edges[0]);
		cornerNormals[1] = glm::cross(edges[0], edges[1]);
		cornerNormals[2] = glm::cross(edges[1], edges[2]);
		cornerNormals[3] = glm::cross(edges[2], edges[3]);

		glm::vec3 porincipalAxes[2];
		porincipalAxes[0] = edges[0] - edges[2];
		porincipalAxes[1] = edges[1] - edges[3];

		glm::vec3 unitCenterNormal;
		unitCenterNormal = glm::cross(porincipalAxes[0], porincipalAxes[1]);
		unitCenterNormal = glm::normalize(unitCenterNormal);

		areas[0] = glm::dot(unitCenterNormal,cornerNormals[0]);
		areas[1] = glm::dot(unitCenterNormal,cornerNormals[1]);
		areas[2] = glm::dot(unitCenterNormal,cornerNormals[2]);
		areas[3] = glm::dot(unitCenterNormal,cornerNormals[3]);
	}


	static float QuadJacobian(glm::vec3 edges[4])
	{	
		double minScaledJ = DBL_MAX;
		double scaledJac;
		double conerAreas[4];
		SignedCornerAreas(conerAreas, edges);

		double length[4];
		for (int i = 0; i < 4; i++)
			length[i] = glm::length(edges[i]);
		if (length[0] < DBL_MIN || length[1] < DBL_MIN || length[2] < DBL_MIN || length[3] < DBL_MIN)
			return 0.0f;

		scaledJac = conerAreas[0] / (length[0] * length[3]);
		minScaledJ = std::min(scaledJac, minScaledJ);

		scaledJac = conerAreas[1] / (length[1] * length[0]);
		minScaledJ = std::min(scaledJac, minScaledJ);

		scaledJac = conerAreas[2] / (length[2] * length[1]);
		minScaledJ = std::min(scaledJac, minScaledJ);

		scaledJac = conerAreas[3] / (length[3] * length[2]);
		minScaledJ = std::min(scaledJac, minScaledJ);

		if (minScaledJ > 0)
			return (float)std::min(minScaledJ, DBL_MAX);
		return (float)std::max(minScaledJ, -DBL_MAX);
	}


	static float TriJacobian(glm::vec3 edges[3])
	{
		glm::vec3 first = edges[1] - edges[0];
		glm::vec3 second = edges[2] - edges[0];

		glm::vec3 normal = glm::cross(first, second);
		double jacobian = glm::length(normal);

		double length0 = glm::length(edges[0]);
		double length1 = glm::length(edges[1]);
		double length2 = glm::length(edges[2]);


		double maxEdgeLengthProduct = std::max(length0*length1, std::max(length1*length2,length0*length2));
		if (maxEdgeLengthProduct < DBL_MIN)
			return 0.0f;

		jacobian *= (2.0f / sqrt(3.0));
		jacobian /= maxEdgeLengthProduct;
		if(jacobian>0)
			return (float)std::min(jacobian, DBL_MAX);
		return (float)std::max(jacobian, -DBL_MAX);
	}
	DrawablePolygonMesh::DrawablePolygonMesh(const VERTEXLIST& vertices, const CELLLIST& cells_or_faces,uint32_t id)
	{
		SetId(id);
		LoadData(vertices, cells_or_faces);
	}
	void DrawablePolygonMesh::LoadData(
		const VERTEXLIST&		 vertices,
		const CELLLIST&		 cells_or_faces
	)
	{
		m_Vertices.reserve(vertices.size());
		uint32_t count = 0;

	
		for (const auto& vpos : vertices)
			m_Vertices.emplace_back(count++, vpos[0], vpos[1], vpos[2]);


		m_Faces.reserve(cells_or_faces.size());
		count = 0;
		for (const auto& faces : cells_or_faces)
		{
			if (faces.size() == 3)
				m_Faces.emplace_back(count++, faces[0], faces[1], faces[2],GetMeshSoildColor());
			else if (faces.size() == 4)
				m_Faces.emplace_back(count++, faces[0], faces[1], faces[2],faces[3],GetMeshSoildColor());
			else
			{
				MESH_ERROR("Read mesh error! Currently only support triangle and quads");
				return;
			}
		}

		switch (cells_or_faces[0].size())
		{
		case 3:
			SetMeshType(MeshType::TRIMESH);
			break;
		case 4:
			SetMeshType(MeshType::QUADMESH);
			break;
		default:
			MESH_ERROR("Read mesh error! Unkonw type");
			break;
		}

		UpdateNormalPerFace();
		UpdateJacobian();
		UpdateScaleFactor();
		UpdateFilter(1.0f, 1.0f, 1.0f, 1.0f);
		//UpdateGL(); included by updateFilter
	}

	void DrawablePolygonMesh::UpdateJacobian()
	{
		auto& jacobian = GetJacobian();
		jacobian.AvgJ = 0.0f;
		jacobian.MinJ = 1.0f;
		jacobian.DeviationJ = 0;
		jacobian.CellJacobian.resize(m_Faces.size(), 0.0f);
		for (auto& face : m_Faces)
		{
			float faceJ = 1.0f;
			if (GetMeshType() == MeshType::QUADMESH)
			{
				glm::vec3 edges[4];
				for (int i = 0; i < 4; i++)
					edges[i] = m_Vertices[face.Indices[(i + 1) % 4]].Position - m_Vertices[face.Indices[i % 4]].Position;
				float scaledJocabian = QuadJacobian(edges);
				faceJ = scaledJocabian;
			}
			else
			{
				//tri
				glm::vec3 edges[3];
				
				edges[0] = m_Vertices[face.Indices[1]].Position - m_Vertices[face.Indices[0]].Position;
				edges[1] = m_Vertices[face.Indices[2]].Position - m_Vertices[face.Indices[0]].Position;
				edges[2] = m_Vertices[face.Indices[2]].Position - m_Vertices[face.Indices[1]].Position;
				float scaledJocabian = TriJacobian(edges);
				faceJ = scaledJocabian;
			}

			jacobian.CellJacobian[face.Id] = faceJ;
			jacobian.AvgJ += faceJ;
			if (jacobian.MinJ > faceJ) jacobian.MinJ = faceJ;
			if (jacobian.MaxJ < faceJ)jacobian.MaxJ = faceJ;
		}

		jacobian.AvgJ /= m_Faces.size();
		for (int i = 0; i < m_Faces.size(); i++)
			jacobian.DeviationJ += (jacobian.CellJacobian[i] - jacobian.AvgJ) * (jacobian.CellJacobian[i] - jacobian.AvgJ);

		jacobian.DeviationJ /= m_Faces.size();
	}


	void DrawablePolygonMesh::UpdateGL()
	{
		
		const auto& filter = GetFilter();
		std::vector<RenderVertex> vertices;
		vertices.reserve(m_Faces.size() * 4);
		uint32_t count = 0;
		std::vector<uint32_t> indices;
		indices.reserve(m_Faces.size() * 4);
		for (const auto& face : m_Faces)
		{
			if (face.IsFilter) continue;
			size_t size = face.Indices.size();
			for (int i = 0; i < size; i++)
			{
				vertices.emplace_back(m_Vertices[face.Indices[i]].Position, face.Color, m_Vertices[face.Indices[i]].Normal,(int)GetId());
				indices.emplace_back(count++);
			}
		}

		auto& vao = GetVertexArrayBuffer();
		
		if (vertices.empty() || indices.empty())
		{
			vao->GetIndexBuffer()->SetCount(0);
			return;
		}
			
		Ref<VertexBuffer> vb = VertexBuffer::Create(&vertices[0], (uint32_t)(vertices.size()*sizeof(RenderVertex)));
		BufferLayout layout =
		{
			{"a_Position",OpenGLDataType::Float,3},
			{"a_Color",OpenGLDataType::Float,3},
			{"a_Normal",OpenGLDataType::Float,3},
			{"a_EntityId",OpenGLDataType::Int,1}
		};

		vertices.shrink_to_fit();
		indices.shrink_to_fit();

		vb->SetLayout(layout);
		Ref<IndexBuffer> ib = IndexBuffer::Create(&indices[0], (uint32_t)indices.size());

		vao->SetVertexBuffer(vb);
		vao->SetIndexBuffer(ib);

	}

	void DrawablePolygonMesh::UpdateBoundingBox()
	{
		float min[3] = { m_Vertices[0].Position[0],m_Vertices[0].Position[1],m_Vertices[0].Position[2] };
		float max[3] = { m_Vertices[0].Position[0],m_Vertices[0].Position[1],m_Vertices[0].Position[2] };
		for (auto& v : m_Vertices)
		{
			min[0] = (v.Position[0] < min[0]) ? v.Position[0] : min[0];
			min[1] = (v.Position[1] < min[1]) ? v.Position[1] : min[1];
			min[2] = (v.Position[2] < min[2]) ? v.Position[2] : min[2];
			max[0] = (v.Position[0] > max[0]) ? v.Position[0] : max[0];
			max[1] = (v.Position[1] > max[1]) ? v.Position[1] : max[1];
			max[2] = (v.Position[2] > max[2]) ? v.Position[2] : max[2];
		}
		SetBoundingBox(min[0], min[1], min[2], max[0], max[1], max[2]);
	}

	void DrawablePolygonMesh::UpdateFilter(float x, float y, float z, float jacobian)
	{
		auto& filter = GetFilter();
		filter.SetLimitX(x);
		filter.SetLimitY(y);
		filter.SetLimitZ(z);
		filter.SetLimitJacobian(jacobian);

		for (auto& face : m_Faces)
			face.IsFilter = filter.IsFilter(this, face);
		UpdateGL();

	}

	void DrawablePolygonMesh::Normalized(glm::mat4& transform)
	{
		glm::vec3 pos = GetFocalPoint();

		transform = glm::translate(transform, pos);
		glm::mat4 inverse = glm::inverse(transform);
		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			glm::vec4 v = glm::vec4(m_Vertices[i].Position ,1.0f );
			v = inverse * v;
			m_Vertices[i].Position.x = v.x;
			m_Vertices[i].Position.y = v.y;
			m_Vertices[i].Position.z = v.z;
		}
		UpdateBoundingBox();
		UpdateGL();
	}

	void DrawablePolygonMesh::ResetColor(const glm::vec3& color)
	{
		for (auto& face : m_Faces)
		{
			face.Color = color;
		}
		UpdateGL();
	}

	void DrawablePolygonMesh::SetCellColor(uint32_t id, const glm::vec3& color)
	{
		if (id >= m_Faces.size() || id < 0)
		{
			MESH_INFO("Wrong id input ,out of range!!");
		}
		else
		{
			m_Faces[id].Color = color;
			UpdateGL();
		}
	}

	void DrawablePolygonMesh::SavePl2(const char* filepath, const glm::mat4& transform)
	{
		std::ofstream f(filepath);
		f << m_Faces.size() << " " << m_Vertices.size() << " " << 0<<std::endl;
		for (int i = 0; i < m_Vertices.size(); i++)
		{
			f << i+1 << " " << m_Vertices[i].Position[0] << " " << m_Vertices[i].Position[1] << std::endl;
		}

		for (int i = 0; i < m_Faces.size(); i++)
		{
			f << i+1 << " " << m_Faces[i].Indices[0] + 1 << " " << m_Faces[i].Indices[1] + 1 << " " << m_Faces[i].Indices[2] + 1 << " " << 0 << " " << 0 << std::endl;
		}

		f.close();
	}



	void DrawablePolygonMesh::UpdateNormalPerFace()
	{
		/*auto valid = [](const glm::vec3 vec)
		{
			return !(abs(vec.x - 0.0f < FLT_EPSILON) && abs(vec.y - 0.0f < FLT_EPSILON) && abs(vec.z - 0.0f < FLT_EPSILON));
		};*/
		for (auto& face : m_Faces)
		{
#if 0
			size_t size = face.Indices.size();
			glm::vec3 normal = { 0.0f,0.0f,0.0f };
			for (int i = 0; i < size; i++)
			{
				uint32_t preVid = face.Indices[(i - 1) % size];
				uint32_t nextVid = face.Indices[(i + 1) % size];
				glm::vec3 pre = m_Vertices[face.Indices[i]].Position - m_Vertices[preVid].Position;
				glm::vec3 next = m_Vertices[nextVid].Position - m_Vertices[face.Indices[i]].Position;

				if (!valid(pre) || !valid(next))
					continue;

				normal += glm::normalize(glm::cross(pre,next));
			}
			face.Normal = glm::normalize(normal);
#endif
			//shoule be fix!
			face.Normal = glm::normalize(
				glm::cross(m_Vertices[face.Indices[1]].Position - m_Vertices[face.Indices[0]].Position, m_Vertices[face.Indices[2]].Position - m_Vertices[face.Indices[1]].Position)
			);
			for (int i = 0; i < face.Indices.size(); i++)
				m_Vertices[face.Indices[i]].Normal += face.Normal;
		}

		for (auto& v : m_Vertices)
		{
			v.Normal = glm::normalize(v.Normal);
		}
		
	}
}