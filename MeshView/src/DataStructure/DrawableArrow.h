#pragma once

#include"glm/glm.hpp"
#include"Renderer/Shader.h"
#include"Renderer/VertexArrayBuffer.h"
namespace MV
{
	class DrawableArrow
	{
	public:
		DrawableArrow() = default;
		DrawableArrow(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& color, float length, float thickness);
		DrawableArrow(glm::vec3&& pos, glm::vec3&& dir, glm::vec3&& color, float length, float thickness);
			

		Ref<VertexArrayBuffer> GetVertexArrayBuffer()const { return m_VertexArrayBuffer; }
		void Build();




		inline void SetThinkness(float thickness) { m_Thickness = thickness; }
		inline void SetLength(float length) { m_Length = length; }
		inline void SetPos(const glm::vec3& pos) { m_Pos = pos; }
		inline void SetDirection(const glm::vec3& dir) { m_Direction = dir; }
		inline void SetColor(const glm::vec3& color) { m_Color = color; }
		inline const glm::mat4& GetRotation()const { return m_Rotation; }
		inline float GetThinkness()const { return m_Thickness; }
		inline float GetLength()const { return m_Length; }
		inline const glm::vec3 GetColor()const{return m_Color;}
		inline const glm::vec3& GetPos()const { return m_Pos; }
		inline const glm::vec3& GetDirection()const { return m_Direction; }
		inline const glm::mat4& GetTranslate()const { return m_Translate; }
	private:

		//calculte pos{0.0f,0.0f,0.0f} dir{0.0,0.0,1.0}
		void CalculateVertex(std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices);

		void CalculateRotation();

	private:
		float m_Thickness;
		float m_Length;
		glm::vec3 m_Pos;
		glm::vec3 m_Direction;
		glm::vec3 m_Color;
		glm::mat4 m_Rotation = glm::mat4(1.0f);
		glm::mat4 m_Translate = glm::mat4(1.0f);
		
		Ref<VertexArrayBuffer> m_VertexArrayBuffer= std::make_shared<VertexArrayBuffer>();
	};


}