#pragma once
#include<string>
#include<map>

#include"../Entity/Entity.h"
namespace MV
{
	enum FileType
	{
		OBJ = 0,
		OFF,
		VTK,
		PL2,


		//My Cutom file
		LS,     //lines file.
		PS		//points file.
	};

	struct FileTypeMap
	{
	public:
		FileTypeMap() {
			Map["obj"] = OBJ;
			Map["off"] = OFF;
			Map["vtk"] = VTK;
			Map["pl2"] = PL2;
			Map["ls"]  = LS;
			Map["ps"]  = PS;
		}			   

		FileType operator[](const std::string& suffix)
		{
			return Map[suffix];
		}
	private:
		std::map<std::string, FileType> Map;
	};

	class DrawableMesh;
	class MeshFileLoader
	{
	public:
		MeshFileLoader() = delete;
		
	
		static Entity LoadFile(const std::string& filename) { return ParseFile(filename); }
		static Entity ParseFile(const std::string& filename);

		static Entity LoadObj(std::stringstream& fileContent,const std::string& name);
		static Entity LoadOff(std::stringstream& fileContent,const std::string& name);
		static Entity LoadVtk(std::stringstream& fileContent,const std::string& name);
		static Entity LoadPl2(std::stringstream& fileContent, const std::string& name);
		

		static void SaveFile(Entity entity, const std::string& filename);

		// my custom file 
		static Entity LoadLS(std::stringstream& fileContent, const std::string& name);
		static Entity LoadPS(std::stringstream& fileContent, const std::string& name);
		//
	private:
		static FileTypeMap s_FileTypeMap;

	};

}