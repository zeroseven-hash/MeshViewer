#include"pch.h"
#include "DrawablePolyhedralMesh.h"

#include<fstream>
#include<map>
#include<set>

#include"DrawablePolygonMesh.h"
#include<glm/glm.hpp>
#include"glm/gtx/transform.hpp"



namespace MV
{
	const int s_HexTetTable[8][4] =
	{
		{0,4,1,3}, //0
		{4,0,7,5}, //4
		{5,6,1,4}, //5
		{1,2,0,5}, //1
		{3,2,7,0}, //3
		{7,6,4,3}, //7
		{6,5,7,2}, //6
		{2,3,1,6}, //2
	};

	static float ComputeTetJacobian(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3)
	{
		glm::mat3 jacobian;
		jacobian[0] = v1 - v0;
		jacobian[1] = v2 - v0;
		jacobian[2] = v3 - v0;
	


		double scaledJacobian = glm::determinant(jacobian);
		
		double norm1 = glm::length(jacobian[0]);
		double norm2 = glm::length(jacobian[1]);
		double norm3 = glm::length(jacobian[2]);

		if (norm1 < DBL_MIN || norm2 < DBL_MIN || norm3 < DBL_MIN) return 0.0f;

		scaledJacobian /= norm1*norm2*norm3;
		return scaledJacobian;


	}

	Cell::Cell(uint32_t id, const uint32_t* vs, int n,const glm::vec3& color)
		:Id(id)
	{
		switch (n)
		{
		case 8:
		{
			const int HexFaceId[6][4] =
			{
				{1,0,3,2},
				{4,5,6,7},
				{4,0,1,5},
				{7,6,2,3},
				{1,2,6,5},
				{3,0,4,7}
			};
			Faces.reserve(6);
			Vs.assign(vs, vs + 8);
			for (int i = 0; i < 6; i++)
				Faces.emplace_back(id*6 + i, vs[HexFaceId[i][0]], vs[HexFaceId[i][1]], vs[HexFaceId[i][2]], vs[HexFaceId[i][3]],color);
			break;
		}
		case 4:
		{
			const int TetFaceId[4][3] =
			{
				{1,0,2},
				{3,2,0},
				{1,2,3},
				{0,1,3}
			};
			Faces.reserve(4);
			Vs.assign(vs, vs + 4);
			for (int i = 0; i < 4; i++)
				Faces.emplace_back(id*4 + i, vs[TetFaceId[i][0]], vs[TetFaceId[i][1]], vs[TetFaceId[i][2]],color);
			break;
		}
		default:
			IGAME_ASSERT(false, "unkone mesh type");
			break;
		}


	}


	DrawablePolyhedralMesh::DrawablePolyhedralMesh(const VERTEXLIST& vertices, const CELLLIST& cells,uint32_t id)
	{
		SetId(id);
		LoadData(vertices, cells);
	}

	void DrawablePolyhedralMesh::LoadData(
		const VERTEXLIST&		 vertices, 
		const CELLLIST&			 cells
	)
	{
		m_Vertices.reserve(vertices.size());
		uint32_t count = 0;

		for (const auto& vpos : vertices)
			m_Vertices.emplace_back(count++, vpos[0], vpos[1], vpos[2]);



		m_Cells.reserve(cells.size());
		count = 0;
		for (const auto& cell : cells)
		{
			if (cell.size() == 8)
				m_Cells.emplace_back(count++, cell.data(), 8, GetMeshSoildColor());
			else if (cell.size() == 4)
				m_Cells.emplace_back(count++, cell.data(), 4, GetMeshSoildColor());
			else
			{
				MESH_ERROR("Read mesh error! Currently only support triangle and quads");
				return;
			}
		}

		switch (cells[0].size())
		{
		case 8:
			SetMeshType(MeshType::HEX);
			break;
		case 4:
			SetMeshType(MeshType::TET);
			break;
		default:
			MESH_ERROR("Read mesh error! Unkonw type");
			break;
		}



		UpdateNormalPerFace();
		UpdateJacobian();
		UpdateScaleFactor();
		UpdateFilter(1.0f, 1.0f, 1.0f, 1.0f);

		//UpdateGL();  included by updateFilter
	}

	void DrawablePolyhedralMesh::UpdateJacobian()
	{
		auto& jacobian = GetJacobian();
		jacobian.AvgJ = 0.0f;
		jacobian.MinJ = 1.0f;
		jacobian.DeviationJ = 0;
		jacobian.CellJacobian.resize(m_Cells.size(), 0.0f);
		
		for (auto& cell : m_Cells)
		{
			if (GetMeshType() == MeshType::HEX)
			{
				double cellJ = 1.0;
				for (uint32_t i = 0; i < 8; i++)
				{
					uint32_t v0, v1, v2, v3;
					v0 = cell.Vs[s_HexTetTable[i][0]]; v1 = cell.Vs[s_HexTetTable[i][1]];
					v2 = cell.Vs[s_HexTetTable[i][2]]; v3 = cell.Vs[s_HexTetTable[i][3]];
					double scaledJacobian = ComputeTetJacobian(m_Vertices[v0].Position, m_Vertices[v1].Position, m_Vertices[v2].Position, m_Vertices[v3].Position);

					if (cellJ > scaledJacobian) cellJ = scaledJacobian;
				}
				jacobian.CellJacobian[cell.Id] = cellJ;
				jacobian.AvgJ += cellJ;
				if (jacobian.MinJ > cellJ) jacobian.MinJ = cellJ;
				if (jacobian.MaxJ < cellJ)jacobian.MaxJ = cellJ;
			}
			else
			{
				//Tet
				uint32_t v0, v1, v2, v3;
				v0 = cell.Vs[0]; v1 = cell.Vs[1];
				v2 = cell.Vs[2]; v3 = cell.Vs[3];
				double scaledJacobian = ComputeTetJacobian(m_Vertices[v0].Position, m_Vertices[v1].Position, m_Vertices[v2].Position, m_Vertices[v3].Position);
				jacobian.CellJacobian[cell.Id] = scaledJacobian;
				jacobian.AvgJ += scaledJacobian;
				if (jacobian.MinJ > scaledJacobian) jacobian.MinJ = scaledJacobian;
				if (jacobian.MaxJ < scaledJacobian)jacobian.MaxJ = scaledJacobian;
			}
		}


		jacobian.AvgJ /= m_Cells.size();
		for (int i = 0; i < m_Cells.size(); i++)
			jacobian.DeviationJ += (jacobian.CellJacobian[i] - jacobian.AvgJ) * (jacobian.CellJacobian[i] - jacobian.AvgJ);

		jacobian.DeviationJ /= m_Cells.size();
	}

	void DrawablePolyhedralMesh::UpdateGL()
	{
		int n = (GetMeshType() == MeshType::HEX) ? 6 : 4;
		uint32_t faceNum = (uint32_t)m_Cells.size() * n;
		std::vector<RenderVertex> vertices;
		vertices.reserve(faceNum * 4);
		uint32_t count = 0;
		std::vector<uint32_t> indices;
		indices.reserve(faceNum * 4);
		for (const auto& cell : m_Cells)
		{
			for (const auto& face : cell.Faces)
			{
				if (!face.IsBoundary) continue;
				
				size_t size = face.Indices.size();
				for (int i = 0; i < size; i++)
				{
					vertices.emplace_back(m_Vertices[face.Indices[i]].Position, face.Color, m_Vertices[face.Indices[i]].Normal, GetId());
					indices.emplace_back(count++);
					
				}
			}
		}
		indices.shrink_to_fit();
		vertices.shrink_to_fit();

		auto& vao = GetVertexArrayBuffer();

		if (indices.empty() || vertices.empty())
		{
			vao->GetIndexBuffer()->SetCount(0);
			return;
		}
		Ref<VertexBuffer> vb = VertexBuffer::Create(&vertices[0], (uint32_t)(vertices.size() * sizeof(RenderVertex)));
		BufferLayout layout =
		{
			{"a_Position",OpenGLDataType::Float,3},
			{"a_Color",OpenGLDataType::Float,3},
			{"a_Normal",OpenGLDataType::Float,3},
			{"a_EntityId",OpenGLDataType::Int,1}
		};

		vb->SetLayout(layout);
		Ref<IndexBuffer> ib = IndexBuffer::Create(&indices[0], (uint32_t)indices.size());

		vao->SetVertexBuffer(vb);
		vao->SetIndexBuffer(ib);

	}

	void DrawablePolyhedralMesh::UpdateBoundingBox()
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

	void DrawablePolyhedralMesh::UpdateFilter(float x, float y, float z, float jacobian)
	{
		auto& filter = GetFilter();
		filter.SetLimitX(x);
		filter.SetLimitY(y);
		filter.SetLimitZ(z);
		filter.SetLimitJacobian(jacobian);
		for (auto& cell : m_Cells)
		{
			
			bool isFilter = filter.IsFilter(this, cell);
			for (auto& face : cell.Faces)
			{
				face.IsFilter = isFilter;
			}
		}
		TagBoundaryFace();

		//update normal
		for (auto& cell : m_Cells)
		{
			for (auto& f : cell.Faces)
			{
				for (int i = 0; i < f.Indices.size(); i++)
				{
					if (f.IsBoundary)
						m_Vertices[f.Indices[i]].Normal += f.Normal;
				}
			}
		}
		for (auto& v : m_Vertices)
		{
			v.Normal = glm::normalize(v.Normal);
		}
		UpdateGL();
	}

	void DrawablePolyhedralMesh::Normalized(glm::mat4& transform)
	{
		glm::vec3 pos = GetFocalPoint();

		transform = glm::translate(transform, pos);
		glm::mat4 inverse = glm::inverse(transform);
		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			glm::vec4 v = glm::vec4(m_Vertices[i].Position, 1.0f);
			v = inverse * v;
			m_Vertices[i].Position.x = v.x;
			m_Vertices[i].Position.y = v.y;
			m_Vertices[i].Position.z = v.z;
		}
		UpdateBoundingBox();
		UpdateGL();
	}

	void DrawablePolyhedralMesh::ResetColor(const glm::vec3& color)
	{
		for (auto& cell : m_Cells)
		{
			for (auto& face : cell.Faces)
			{
				face.Color = color;
			}
		}
		
		UpdateGL();
	}

	void DrawablePolyhedralMesh::SetCellColor(uint32_t id, const glm::vec3& color)
	{
		if (id >= m_Cells.size() || id < 0)
		{
			MESH_INFO("Wrong id input ,out of range!!");
		}
		else
		{
			for (auto& face : m_Cells[id].Faces)
			{
				face.Color = color;
			}
			UpdateGL();
		}
	}

	void DrawablePolyhedralMesh::SaveVtk(const char* filepath, const glm::mat4& transform)
	{
		std::ofstream f(filepath);

		f << "# vtk DataFile Version 2.0" << std::endl << "mesh vtk data - converted from .off" << std::endl;
		f << "ASCII" << std::endl;
		f << "DATASET UNSTRUCTURED_GRID" << std::endl;

		f << "POINTS " << m_Vertices.size() <<" " << "double" << std::endl;
		for (uint32_t i = 0; i < m_Vertices.size(); i++)
		{
			glm::vec3 pos = glm::vec3(transform*glm::vec4(m_Vertices[i].Position,1.0f));
			f << pos[0] << " " << pos[1] << " " << pos[2] << "\n";
		}
		uint32_t vnum = m_Cells[0].Vs.size();
		f << "CELLS" <<" " << m_Cells.size() << " " << m_Cells.size() * (vnum + 1) << "\n";
		for (UINT32 i = 0; i < m_Cells.size(); i++)
		{
			f << vnum ;
			auto& vs = m_Cells[i].Vs;
			for (int j = 0; j < vnum; j++)
				f << " " << vs[j];
			f << "\n";
		}

		f << "CELL_TYPES" <<" " << m_Cells.size() << "\n";
		for (uint32_t i = 0; i < m_Cells.size(); i++)
			f << 12 << "\n";

		f.close();
	}

	void DrawablePolyhedralMesh::UpdateNormalPerFace()
	{
		for (auto& cell : m_Cells)
		{
			for (auto& f : cell.Faces)
			{
				//should be fix!!!
				f.Normal = glm::normalize(
					glm::cross(m_Vertices[f.Indices[1]].Position - m_Vertices[f.Indices[0]].Position, m_Vertices[f.Indices[2]].Position - m_Vertices[f.Indices[1]].Position));
				
				for (int i = 0; i < f.Indices.size(); i++)
				{
					if(f.IsBoundary)
						m_Vertices[f.Indices[i]].Normal += f.Normal;
				}
			}
		}
		for (auto& v : m_Vertices)
		{
			v.Normal = glm::normalize(v.Normal);
		}
	}


	
	void DrawablePolyhedralMesh::TagBoundaryFace()
	{
		int n = (GetMeshType() == MeshType::HEX) ? 6 : 4;
		std::unordered_map<Face, std::vector<std::pair<uint32_t, int>>, FaceHashFunc> map;
		
	
		for (auto& cell : m_Cells)
		{
			for (auto& face : cell.Faces)
			{
				//init
				face.IsBoundary = false;
				if (face.IsFilter) continue;
				uint32_t eid = face.Id / n;
				int local = face.Id % n;
				map[face].push_back(std::pair(eid, local));
			}
		}
		
		int count = 0;
		for (auto iter = map.begin(); iter != map.end(); iter++)
		{
			if (iter->second.size() == 2)
			{
				m_Cells[iter->second[0].first].Faces[iter->second[0].second].IsBoundary = false;
				m_Cells[iter->second[1].first].Faces[iter->second[1].second].IsBoundary = false;
			}
			else if (iter->second.size() == 1)
			{
				m_Cells[iter->second[0].first].Faces[iter->second[0].second].IsBoundary = true;
			}
			else
				assert(false);
		}


		
	}



}
