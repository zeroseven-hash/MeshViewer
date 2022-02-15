#pragma once

#include"DrawableObject.h"
namespace MV
{
	struct Point
	{
		//temp
		glm::vec3 Pos;


		//TODO color...
		Point(const glm::vec3& pos) :Pos(pos){}
	};
	class DrawablePoint:public DrawableObject
	{
	public:
		DrawablePoint() = default;
		~DrawablePoint() = default;
		DrawablePoint(std::vector<Point>&& points);
		DrawablePoint(const std::vector<Point>& points);


		virtual const Ref<VertexArrayBuffer>& GetVertexArrayBuffer()const override { return m_VertexArrayBuffer; }
		virtual glm::vec3 GetFocalPoint()const override { return glm::vec3((m_Bbox[0][1] + m_Bbox[0][0]) / 2, (m_Bbox[1][1] + m_Bbox[1][0]) / 2, (m_Bbox[2][1] + m_Bbox[2][0]) / 2); }
		virtual float GetScaleFactor()const override { return m_ScaleFactor; }
		virtual ObjectType GetObjectType()const override;
		virtual void Normalized(glm::mat4& transform = glm::mat4(1.0f))override;

		const glm::vec3& GetPointColor()const { return m_PointColor; }
		float GetAlpha()const { return m_Alpha; }
		float GetSize()const { return m_Size; }
		size_t GetPointNum()const { return m_Points.size(); }

		uint32_t GetId()const override { return m_Id; }
		void SetId(uint32_t id)override { m_Id = id; }
		void UpdateScaleFactor() { UpdateBoundingBox(); m_ScaleFactor = std::max(m_Bbox[0][1] - m_Bbox[0][0], std::max(m_Bbox[1][1] - m_Bbox[1][0], m_Bbox[2][1] - m_Bbox[2][0])); }
		void SetPointColor(const glm::vec3& color) { m_PointColor = color; }
		void SetSize(float size) { m_Size = size; }
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
		void UpdateBoundingBox();
		void UpdateGL();
	private:

	private:
		glm::vec3 m_PointColor = { 0.0f,1.0f,0.0f };


		
		float m_Size = 10.0f;
		std::vector<Point> m_Points;
	};
}