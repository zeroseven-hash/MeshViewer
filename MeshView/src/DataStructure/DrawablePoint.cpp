#include"pch.h"
#include"DrawablePoint.h"
#include"glm/gtx/transform.hpp"

namespace MV
{
	DrawablePoint::DrawablePoint(std::vector<Point>&& points)
		:m_Points(std::move(points))
	{
		m_VertexArrayBuffer = std::make_shared<VertexArrayBuffer>();
		
		UpdateScaleFactor();
		UpdateGL();
	}
	DrawablePoint::DrawablePoint(const std::vector<Point>& points)
		:m_Points(points)
	{
		m_VertexArrayBuffer = std::make_shared<VertexArrayBuffer>();
		UpdateScaleFactor();
		UpdateGL();
	}
	ObjectType DrawablePoint::GetObjectType() const
	{
		return ObjectType::POINT;
	}
	void DrawablePoint::Normalized(glm::mat4& transform)
	{
		glm::vec3 pos = GetFocalPoint();
		transform = glm::translate(transform, pos);
		glm::mat4 inverse = glm::inverse(transform);
		for (auto& point : m_Points)
		{
			glm::vec4 v1 = { point.Pos ,1.0f };
			
			v1 = inverse * v1;
			point.Pos = v1;

		}
		UpdateBoundingBox();
		UpdateGL();
	}
	void DrawablePoint::UpdateBoundingBox()
	{
		float min[3] = { FLT_MAX,FLT_MAX,FLT_MAX };
		float max[3] = { FLT_MIN,FLT_MIN ,FLT_MIN };
		for (const auto& p : m_Points)
		{
			const auto& v1 = p.Pos;
			min[0] = (v1[0] < min[0]) ? v1[0] : min[0];
			min[1] = (v1[1] < min[1]) ? v1[1] : min[1];
			min[2] = (v1[2] < min[2]) ? v1[2] : min[2];

			max[0] = (v1[0] > max[0]) ? v1[0] : max[0];
			max[1] = (v1[1] > max[1]) ? v1[1] : max[1];
			max[2] = (v1[2] > max[2]) ? v1[2] : max[2];
		}

		SetBoundingBox(min[0], min[1], min[2], max[0], max[1], max[2]);
	}
	void DrawablePoint::UpdateGL()
	{
		std::vector<glm::vec3> vertices;
		vertices.reserve(m_Points.size());
		uint32_t count = 0;
		std::vector<uint32_t> indices;
		indices.reserve(m_Points.size());
		for (const auto& p : m_Points)
		{
			vertices.emplace_back(p.Pos);
			indices.emplace_back(count++);
		}

		auto& vao = GetVertexArrayBuffer();
		Ref<VertexBuffer> vb = VertexBuffer::Create(&vertices[0], (uint32_t)(vertices.size() * sizeof(glm::vec3)));
		BufferLayout layout =
		{
			{"a_Position",OpenGLDataType::Float,3}
		};

		vb->SetLayout(layout);
		Ref<IndexBuffer> ib = IndexBuffer::Create(&indices[0], (uint32_t)indices.size());

		vao->SetVertexBuffer(vb);
		vao->SetIndexBuffer(ib);
	}
}