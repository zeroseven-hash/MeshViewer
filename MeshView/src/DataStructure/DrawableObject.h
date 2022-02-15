#pragma once
#include"../Renderer/VertexArrayBuffer.h"
#include<glm/glm.hpp>
namespace MV
{
	enum class ObjectType
	{
		MESH=0,
		LINE,
		POINT
	};


	class DrawableObject
	{
	public:
		DrawableObject()=default;
		virtual ~DrawableObject(){}

		
		virtual const Ref<VertexArrayBuffer>& GetVertexArrayBuffer()const = 0;
		virtual glm::vec3 GetFocalPoint()const = 0;
		virtual float GetScaleFactor()const = 0;
		virtual ObjectType GetObjectType()const = 0;
		virtual uint32_t GetId()const = 0;
		virtual void SetId(uint32_t id) = 0;
		virtual void Normalized(glm::mat4& transform = glm::mat4(1.0f)) = 0;

	protected:
		uint32_t m_Id;
		Ref<VertexArrayBuffer> m_VertexArrayBuffer;
		float m_Bbox[3][2] = { 0 };
		float m_Alpha = 1.0f;
		float m_ScaleFactor = 1.0f;
	};
}