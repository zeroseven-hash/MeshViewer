#include "pch.h"
#include "MeshFileLoader.h"

#include<fstream>
#include<algorithm>
#include<sstream>

#include"CoreModel/Log.h"
#include"DrawablePolygonMesh.h"
#include"DrawablePolyhedralMesh.h"
#include"DrawableLine.h"
#include"DrawablePoint.h"
#include"Entity/EntityFactory.h"
#include"Entity/Entity.h"

//temp

namespace MV
{
	FileTypeMap MeshFileLoader::s_FileTypeMap = FileTypeMap();
	Entity MeshFileLoader::ParseFile(const std::string& filename)
	{
	
		size_t pos = filename.find_last_of('.');
		std::string suffix = filename.substr(pos+1, filename.size() - pos-1);
		pos = filename.find_last_of("/\\");
		std::string name = filename.substr(pos + 1);
		std::transform(suffix.begin(), suffix.end(), suffix.begin(),[](unsigned char c) {return std::tolower(c); });

		std::string result;
		std::stringstream iss;
		std::ifstream inFile(filename, std::ios::in|std::ios::binary);
		if (inFile.is_open())
		{
			inFile.seekg(0, std::ios::end);
			std::streampos size = inFile.tellg();
			result.resize(size);
			inFile.seekg(0, std::ios::beg);
			inFile.read(&result[0], size);
			iss.str(result);
		}
		else
		{
			MESH_ERROR("Could not open file: {0}", filename); 
			return Entity();
		}
			

		switch (s_FileTypeMap[suffix])
		{
		case VTK:return LoadVtk(iss, name);
		case OBJ:return LoadObj(iss, name);
		case OFF:return LoadOff(iss, name);
		case PL2:return LoadPl2(iss, name);
		case LS :return LoadLS (iss, name);
		case PS :return LoadPS (iss, name);
		default:
			MESH_ERROR("Unknow file type!");
			return Entity();
		}
		
	}

	Entity MeshFileLoader::LoadObj(std::stringstream& fileContent, const std::string& name)
	{
		VERTEXLIST vertices;
		CELLLIST cells;
		std::string word;
		while (fileContent >> word)
		{
			if (word == "v")
			{
				std::array<float, 3> pos;
				float data;
				for (int i = 0; i < 3; i++)
				{
					fileContent >> data;
					pos[i] = data;
				}
				vertices.emplace_back(pos);
				
			}
			else if(word=="f")
			{
				
			
				char result[256];

				
				fileContent.getline(result, 256);
				std::vector<uint32_t> face;
				char* tok = strtok(result, " ");
				auto isFind = [&](char c,char* pos)
				{
					char* end = tok + sizeof(tok) / sizeof(tok[0]);
					pos = std::find(tok, end, c);
					return(pos != end);
				};

				while (tok != NULL)
				{
					char* pos=nullptr;
					if (!isFind('/',pos))
					{
						uint32_t faceid = (uint32_t)_atoi64(tok);
						face.push_back(faceid - 1);
					}
					else
					{
						pos = '\0';
						uint32_t faceid = (uint32_t)_atoi64(tok);
						face.push_back(faceid - 1);
					}
					tok = strtok(NULL, " ");
				}

				cells.emplace_back(std::move(face));
				
			}
		}
		 
		MESH_INFO("Read Mesh {0}: Vertices {1} ; Faces {2}", name, vertices.size(), cells.size());
		Entity entity = EntityFactory::CreateEntity(name);
		Ref<DrawablePolygonMesh> mesh = std::make_shared<DrawablePolygonMesh>(vertices, cells,(uint32_t)entity);
		glm::mat4 transform = glm::mat4(1.0f);
		mesh->Normalized(transform);
		entity.AddComponent<DrawableComponent>(mesh);
		entity.GetComponent<TransformComponent>().Transform = transform;
		return entity;
	}

	Entity MeshFileLoader::LoadOff(std::stringstream& fileContent, const std::string& name)
	{
		VERTEXLIST vertices;
		CELLLIST cells;
		std::string word;
		fileContent >> word;
		size_t vertices_num = 0;
		size_t faces_num = 0;
		size_t edges_num = 0;
		fileContent >> vertices_num >> faces_num>>edges_num;
		vertices.resize(vertices_num);
		cells.resize(faces_num);
		for (size_t i = 0; i < vertices_num; i++)
		{
			float x, y, z;
			fileContent >> x >> y >> z;
			vertices[i][0] = x;
			vertices[i][1] = y;
			vertices[i][2] = z;
		}
		for (size_t i = 0; i < faces_num; i++)
		{
			int cell_size;
			fileContent >> cell_size;
			if (cell_size != 3&&cell_size != 4)
			{
				MESH_INFO("Read Off file failed! file format is wrong!");
				return Entity();
			}
			cells[i].resize(cell_size);
			for (int j = 0; j < cell_size; j++)
			{
				uint32_t index;
				fileContent >> index;
				cells[i][j] = index;
			}
		}

		MESH_INFO("Read Mesh {0}: Vertices {1} ; Faces {2}.", name, vertices.size(), cells.size());
		Entity entity = EntityFactory::CreateEntity(name);
		Ref<DrawablePolygonMesh> mesh = std::make_shared<DrawablePolygonMesh>(vertices, cells,(uint32_t)entity);
		glm::mat4 transform = glm::mat4(1.0f);
		mesh->Normalized(transform);
		entity.AddComponent<DrawableComponent>(mesh);
		entity.GetComponent<TransformComponent>().Transform = transform;
		return entity;
	}

	Entity MeshFileLoader::LoadVtk(std::stringstream& fileContent, const std::string& name)
	{
		VERTEXLIST vertices;
		CELLLIST cells;
		int cell_type=0;
		char line[256];
		while (fileContent.getline(line, 256))
		{
			char* tok = strtok(line, " ");
			if (!strcmp(tok, "POINTS"))
			{
				tok=strtok(NULL, " ");
				uint32_t size = (uint32_t)_atoi64(tok);
				vertices.resize(size);
				for (size_t i = 0; i < size; i++)
				{
					fileContent.getline(line, 256);
					tok = strtok(line, " ");
					int j = 0;
					while (tok != NULL)
					{
						vertices[i][j++] = (float)std::atof(tok);
						tok=strtok(NULL, " ");
					}
				}
			}
			else if (!strcmp(tok, "CELLS"))
			{
				char* tok = strtok(NULL, " ");
				uint32_t size = (uint32_t)_atoi64(tok);
				cells.reserve(size);
				for (size_t i = 0; i < size; i++)
				{
					fileContent.getline(line, 256);
					tok = strtok(line, " ");
					int count = atoi(tok);
					if (count == 4 || count == 3 || count == 8)
					{
						int j = 0;
						std::vector<uint32_t> cell(count);
						tok = strtok(NULL, " ");
						while(tok != NULL&&j<8)
						{
							cell[j++] = (uint32_t)_atoi64(tok);
							tok = strtok(NULL, " ");
						}
						cells.emplace_back(std::move(cell));
					}
				
				}
			}
			else if (!strcmp(tok, "CELL_TYPES"))
			{
				char* tok = strtok(NULL, " ");
				uint32_t size = (uint32_t)_atoi64(tok);
				for (size_t i = 0; i < size; i++)
				{
					fileContent.getline(line, 256);
					int type = atoi(line);
					cell_type = (cell_type >= type) ? cell_type : type;
				}
			}
		}

		
		switch (cell_type)
		{
		case 5:
		case 9:
		{
			Entity entity = EntityFactory::CreateEntity(name);
			Ref<DrawablePolygonMesh> mesh = std::make_shared<DrawablePolygonMesh>(vertices, cells,(uint32_t)entity);
			MESH_INFO("Read Mesh {0}: Vertices {1} ; Faces {2}.", name, vertices.size(), cells.size());
			glm::mat4 transform = glm::mat4(1.0f);
			mesh->Normalized(transform);
			entity.AddComponent<DrawableComponent>(mesh);
			entity.GetComponent<TransformComponent>().Transform = transform;
			return entity;
		}
		case 10:
		case 12:
		{
			Entity entity = EntityFactory::CreateEntity(name);
			Ref<DrawablePolyhedralMesh> mesh = std::make_shared<DrawablePolyhedralMesh>(vertices, cells,(uint32_t)entity);
			MESH_INFO("Read Mesh {0}: Vertices {1} ; Cells {2}.", name, vertices.size(), cells.size());
			glm::mat4 transform = glm::mat4(1.0f);
			mesh->Normalized(transform);
			entity.AddComponent<DrawableComponent>(mesh);
			entity.GetComponent<TransformComponent>().Transform = transform;
			return entity;
		}
		default:
			MESH_INFO("Read Vtk file failed! Cell type is not supported!");
			break;
		}
		return Entity();
	}

	Entity MeshFileLoader::LoadPl2(std::stringstream& fileContent, const std::string& name)
	{

		VERTEXLIST vertices;
		CELLLIST cells;
		char line[256];
		
		fileContent.getline(line,256);
		char* tok = strtok(line," ");
		int count[3];
		int i = 0;
		while (tok)
		{
			count[i++] = std::atoi(tok);
			tok = strtok(NULL, " ");
		}

		vertices.resize(count[1]);
		cells.reserve(count[0]);
		for (size_t i = 0; i < count[1]; i++)
		{
			fileContent.getline(line, 256);
			float v[3];
			int j = 0;
			char* tok = strtok(line, " ");
			while (tok)
			{
				v[j++] = (float)std::atof(tok);
				tok = strtok(NULL, " ");
			}
			vertices[i][0]=v[1];
			vertices[i][1] = v[2];
			vertices[i][2] = 0.0f;
		}

		for (size_t i = 0; i < count[0]; i++)
		{
			fileContent.getline(line, 256);
			uint32_t c[6];
			char* tok = strtok(line, " ");
			int j = 0;
			while (tok)
			{
				c[j++]=(uint32_t)_atoi64(tok);
				tok = strtok(NULL, " ");
			}
			std::vector<uint32_t> cell = { c[1]-1,c[2]-1,c[3]-1 };
			cells.emplace_back(std::move(cell));
		}

		Entity entity = EntityFactory::CreateEntity(name);
		Ref<DrawablePolygonMesh> mesh = std::make_shared<DrawablePolygonMesh>(vertices, cells,(uint32_t)entity);
		MESH_INFO("Read Mesh {0}: Vertices£¬{1}, Faces, {2}", name, vertices.size(), cells.size());
		glm::mat4 transform = glm::mat4(1.0f);
		mesh->Normalized(transform);
		entity.AddComponent<DrawableComponent>(mesh);
		entity.GetComponent<TransformComponent>().Transform = transform;
		return entity;
	}

	void  MeshFileLoader::SaveFile(Entity entity, const std::string& filename)
	{
		size_t pos = filename.find_last_of('.');
		std::string suffix = filename.substr(pos + 1, filename.size() - pos - 1);
		pos = filename.find_last_of("/\\");
		std::string name = filename.substr(pos + 1);
		std::transform(suffix.begin(), suffix.end(), suffix.begin(), [](unsigned char c) {return std::tolower(c); });


		Ref<DrawableMesh> mesh_ptr = std::static_pointer_cast<DrawableMesh>(entity.GetComponent<DrawableComponent>().Object);
		glm::mat4& transform = entity.GetComponent<TransformComponent>().Transform;
		switch (s_FileTypeMap[suffix])
		{
		case VTK:mesh_ptr->SaveVtk(filename.c_str(), transform); break;
		case OBJ:assert(false);
		case OFF:assert(false);
		case PL2:mesh_ptr->SavePl2(filename.c_str(), transform); break;
		default:
			MESH_ERROR("Unknow file type!");
		}
	}

	Entity MeshFileLoader::LoadLS(std::stringstream& fileContent, const std::string& name)
	{
		//ls file format:
		// line size:
		// line1: x1 y1 z1 x2 y2 z2;
		// line2:
		// .......
		std::vector<Line> lines;
		
		char line[256];
		fileContent.getline(line, 256);
		uint32_t size= (uint32_t)_atoi64(line);
		lines.reserve(size);
		while (fileContent.getline(line, 256))
		{
			
			float x[6];
			char* tok= strtok(line, " ");
			int count = 0;
			while (tok)
			{
				x[count++] = (float)std::atof(tok);
				tok = strtok(NULL, " ");
			}
			lines.emplace_back(glm::vec3(x[0], x[1], x[2]), glm::vec3(x[3], x[4], x[5]));
		}

		lines.shrink_to_fit();
		Ref<DrawableLine> lineObj = std::make_shared<DrawableLine>(std::move(lines));
		glm::mat4 transform = glm::mat4(1.0f);
		lineObj->Normalized(transform);
		Entity entity = EntityFactory::CreateEntity(name, transform);
		entity.AddComponent<DrawableComponent>(lineObj);
		MESH_INFO("Read Line {0}: Line fragment numbers, {1}.", name,size);

		return entity;
	}

	Entity MeshFileLoader::LoadPS(std::stringstream& fileContent, const std::string& name)
	{
		//Ps file format:
		//points size:
		//point1:x y z
		//Point2:
		//.......
		std::vector<Point> points;

		char line[256];
		fileContent.getline(line, 256);
		uint32_t size = (uint32_t)_atoi64(line);
		points.reserve(size);
		while (fileContent.getline(line, 256))
		{

			float x[3];
			char* tok = strtok(line, " ");
			int count = 0;
			while (tok)
			{
				x[count++] = (float)std::atof(tok);
				tok = strtok(NULL, " ");
			}
			points.emplace_back(glm::vec3(x[0], x[1], x[2]));
		}

		points.shrink_to_fit();
		Ref<DrawablePoint> pointObj = std::make_shared<DrawablePoint>(std::move(points));
		glm::mat4 transform=glm::mat4(1.0f);
		pointObj->Normalized(transform);
		Entity entity = EntityFactory::CreateEntity(name,transform);
		entity.AddComponent<DrawableComponent>(pointObj);
		MESH_INFO("Read Points {0}: Points numbers, {1}.", name, size);

		return entity;
		return Entity();
	}

}
