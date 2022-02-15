#pragma once
#include"DrawableObject.h"
#include"MeshFilter.h"
#include<glm/glm.hpp>
namespace MV
{


	typedef std::vector<std::array<float, 3>>  VERTEXLIST;
	typedef std::vector<std::vector<uint32_t>> CELLLIST;
	
	enum class MeshType
	{
		TRIMESH = 0,
		QUADMESH,
		TET,
		HEX,
		UNKNOW
	};

	enum class RenderMode
	{
		SUFRACE,
		SUFACEWITHEDGES,
		WIREFRAME,
		POINTS,

		//for tet and hex
		VOLUME
	};
	struct RenderVertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec3 Normal;
		int Id;
		RenderVertex(const glm::vec3& pos, const glm::vec3& color, const glm::vec3& normal,int id) :
			Position(pos),
			Color(color),
			Normal(normal),
			Id(id){}
	};
	struct Jacobian
	{
		float MinJ = 0.0f;
		float MaxJ = 0.0f;
		float AvgJ = 0.0f;
		float DeviationJ = 0.0f;
		std::vector<float> CellJacobian;
	};
	
	


	class MeshFilter;
	class DrawableMesh:public DrawableObject
	{
	public:
	
		virtual ~DrawableMesh()=default;

		//Update m_VertexArrayBuffer;
		//virtual void UpdateGL();
		virtual void LoadData(
			const VERTEXLIST&		vertices,
			const CELLLIST&		cells_or_faces
		) 
		{ MESH_ERROR("DrawableMesh should have a sub class"); }
		
		virtual void UpdateGL() { MESH_ERROR("DrawableMesh should have a sub class"); }
		virtual void UpdateBoundingBox(){ MESH_ERROR("DrawableMesh should have a sub class"); }
		virtual void UpdateFilter(float x, float y, float z, float jacobian) { MESH_ERROR("DrawableMesh should have a sub class"); }
		virtual void ResetColor(const glm::vec3& color){ MESH_ERROR("DrawableMesh should have a sub class"); }
		virtual void SetCellColor(uint32_t id, const glm::vec3& color) { MESH_ERROR("DrawableMesh should have a sub class"); }
		virtual size_t GetVertNum() { MESH_ERROR("DrawableMesh should have a sub class"); return 0; }
		virtual size_t GetCellNum() { MESH_ERROR("DrawableMesh should have a sub class"); return 0; }
		virtual void UpdateJacobian() { MESH_ERROR("DrawableMesh should have a sub class"); }
		virtual void Normalized(glm::mat4& transfrom=glm::mat4(1.0f))override{ MESH_ERROR("DrawableMesh should have a sub class"); }

		virtual void SaveVtk(const char* filepath, const glm::mat4& transform = glm::mat4(1.0f)){ MESH_ERROR("DrawableMesh should have a sub class"); }
		virtual void SavePl2(const char* filepath, const glm::mat4& transform=glm::mat4(1.0f)){ MESH_ERROR("DrawableMesh should have a sub class"); }


		void UpdateScaleFactor() { UpdateBoundingBox(); m_ScaleFactor = std::max(m_Bbox[0][1] - m_Bbox[0][0], std::max(m_Bbox[1][1] - m_Bbox[1][0], m_Bbox[2][1] - m_Bbox[2][0])); }


		uint32_t GetId()const override { return m_Id; }
		void SetId(uint32_t id)override { m_Id = id; }
		const Ref<VertexArrayBuffer>& GetVertexArrayBuffer()const override { return m_VertexArrayBuffer; }
		glm::vec3 GetFocalPoint()const override { return glm::vec3((m_Bbox[0][1] + m_Bbox[0][0]) / 2, (m_Bbox[1][1] + m_Bbox[1][0]) / 2, (m_Bbox[2][1] + m_Bbox[2][0]) / 2); }
		float GetScaleFactor()const override { return m_ScaleFactor; }
		ObjectType GetObjectType()const override { return DrawableMesh::ObjectType(); }
		static ObjectType ObjectType() { return ObjectType::MESH; }
		RenderMode GetRenderMode()const { return m_RenderMode; }
		Ref<VertexArrayBuffer>& GetVertexArrayBuffer() { return m_VertexArrayBuffer; }
		const glm::vec3& GetMeshSoildColor()const { return m_MeshSoildColor; }
		const glm::vec3& GetMeshLineColor()const { return m_MeshLineColor; }
		float GetAlpha()const { return m_Alpha; }
		MeshType GetMeshType() { return m_Type; }
		Jacobian& GetJacobian() { return m_Jacobian; }
		const Jacobian& GetJacobian()const { return m_Jacobian; }
		glm::vec3 GetMinBound()const { return glm::vec3(m_Bbox[0][0], m_Bbox[1][0], m_Bbox[2][0]); }
		glm::vec3 GetMaxBound()const { return glm::vec3(m_Bbox[0][1], m_Bbox[1][1], m_Bbox[2][1]); }

		const MeshFilter& GetFilter()const { return m_Filter; }
		MeshFilter& GetFilter() { return m_Filter; }
		
		void SetRenderMode(RenderMode mode) { m_RenderMode = mode; }
		void SetAlpha(float alpha) { m_Alpha = alpha; }
		void SetMeshType(MeshType type) { m_Type = type; }
		void SetMeshSoildColor(const glm::vec3& color) { m_MeshSoildColor = color; ResetColor(color); }
		void SetMeshLineColor(const glm::vec3& color) { m_MeshLineColor = color; }
		void SetBoundingBox(float minx,float miny,float minz,float maxx,float maxy,float maxz)
		{
			m_Bbox[0][0] = minx;
			m_Bbox[0][1] = maxx;
			m_Bbox[1][0] = miny;
			m_Bbox[1][1] = maxy;
			m_Bbox[2][0] = minz;
			m_Bbox[2][1] = maxz;
		}
		
	
	protected:
		DrawableMesh(){ m_VertexArrayBuffer = std::make_shared<VertexArrayBuffer>(); }


	private:

		glm::vec3 m_MeshSoildColor = { 1.0f, 1.0f, 1.0f};
		glm::vec3 m_MeshLineColor = { 0.525f, 0.525f, 0.525f};
		RenderMode m_RenderMode = RenderMode::SUFACEWITHEDGES;
		MeshType m_Type= MeshType::UNKNOW;

		MeshFilter m_Filter;
		Jacobian m_Jacobian;
	};

}