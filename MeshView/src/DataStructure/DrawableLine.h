#pragma once

#include"DrawableObject.h"
namespace MV
{
	struct Line
	{
		glm::vec3 V1;
		glm::vec3 V2;
		Line(const glm::vec3& v1,const glm::vec3& v2):V1(v1),V2(v2){}
	};
	class DrawableLine :public DrawableObject
	{
	public:
		DrawableLine() = default;
		~DrawableLine() = default;
		DrawableLine(std::vector<Line>&& lines);
		DrawableLine(const std::vector<Line>& lines);


		virtual const Ref<VertexArrayBuffer>& GetVertexArrayBuffer()const override { return m_VertexArrayBuffer; }
		virtual glm::vec3 GetFocalPoint()const override { return glm::vec3((m_Bbox[0][1] + m_Bbox[0][0]) / 2, (m_Bbox[1][1] + m_Bbox[1][0]) / 2, (m_Bbox[2][1] + m_Bbox[2][0]) / 2);}
		virtual float GetScaleFactor()const override { return m_ScaleFactor; }
		virtual ObjectType GetObjectType()const override;
		virtual void Normalized(glm::mat4& transform = glm::mat4(1.0f))override;

		const glm::vec3& GetLineColor()const { return m_LineColor; }
		float GetAlpha()const { return m_Alpha; }
		float GetWidth()const { return m_Width; }
		size_t GetLineNum()const { return m_Lines.size(); }

		uint32_t GetId()const override { return m_Id; }
		void SetId(uint32_t id)override { m_Id = id; }
		void SetLineColor(const glm::vec3& color) { m_LineColor = color; }
		void SetWidth(float width) { m_Width = width; }
		void SetAlpha(float alpha) { m_Alpha = alpha; }
		void SetBoundingBox(float minx, float miny, float minz, float maxx, float maxy, float maxz)
		{
			m_Bbox[0][0] = minx;
			m_Bbox[0][1] = maxx;
			m_Bbox[1][0] = miny;
			m_Bbox[1][1] = maxy;
			m_Bbox[2][0] = minz;
			m_Bbox[2][1] = maxz;
		}
		void UpdateScaleFactor() { UpdateBoundingBox(); m_ScaleFactor = std::max(m_Bbox[0][1] - m_Bbox[0][0], std::max(m_Bbox[1][1] - m_Bbox[1][0], m_Bbox[2][1] - m_Bbox[2][0])); }
		void UpdateBoundingBox();
		void UpdateGL();
		
	private:
		glm::vec3 m_LineColor={1.0f,0.0f,0.0f};
		float m_Width = 2.0f;
		std::vector<Line> m_Lines;
	};
}