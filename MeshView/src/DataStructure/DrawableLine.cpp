#include"pch.h"
#include"DrawableLine.h"
#include"glm/gtx/transform.hpp"

namespace MV
{
	DrawableLine::DrawableLine(std::vector<Line>&& lines)
		:m_Lines(std::move(lines))
	{
		m_VertexArrayBuffer = std::make_shared<VertexArrayBuffer>();
		UpdateScaleFactor();
		UpdateGL();
	}
	DrawableLine::DrawableLine(const std::vector<Line>& lines)
		:m_Lines(lines)
	{
		m_VertexArrayBuffer = std::make_shared<VertexArrayBuffer>();
		UpdateScaleFactor();
		UpdateGL();
	}
	
	
	ObjectType DrawableLine::GetObjectType() const
	{
		return ObjectType::LINE;
	}
	void DrawableLine::Normalized(glm::mat4& transform)
	{
		glm::vec3 pos = GetFocalPoint();
		transform = glm::translate(transform, pos);
		glm::mat4 inverse = glm::inverse(transform);
		for (auto& line : m_Lines)
		{
			glm::vec4 v1 = { line.V1 ,1.0f};
			glm::vec4 v2 = { line.V2 ,1.0f};
			v1 = inverse * v1;
			v2 = inverse * v2;
			line.V1 = v1;
			line.V2 = v2;

		}
		UpdateBoundingBox();
		UpdateGL();
	}
	void DrawableLine::UpdateBoundingBox()
	{
		
		float min[3] = { m_Lines[0].V1[0],m_Lines[0].V1[1] ,m_Lines[0].V1[2] };
		float max[3] = { m_Lines[0].V1[0],m_Lines[0].V1[1] ,m_Lines[0].V1[2] };
		for (auto& line : m_Lines)
		{
			auto& v1 = line.V1;
			auto& v2 = line.V2;
			min[0] = (v1[0] < min[0]) ? v1[0] : min[0];
			min[1] = (v1[1] < min[1]) ? v1[1] : min[1];
			min[2] = (v1[2] < min[2]) ? v1[2] : min[2];
			max[0] = (v1[0] > max[0]) ? v1[0] : max[0];
			max[1] = (v1[1] > max[1]) ? v1[1] : max[1];
			max[2] = (v1[2] > max[2]) ? v1[2] : max[2];

			min[0] = (v2[0] < min[0]) ? v2[0] : min[0];
			min[1] = (v2[1] < min[1]) ? v2[1] : min[1];
			min[2] = (v2[2] < min[2]) ? v2[2] : min[2];
			max[0] = (v2[0] > max[0]) ? v2[0] : max[0];
			max[1] = (v2[1] > max[1]) ? v2[1] : max[1];
			max[2] = (v2[2] > max[2]) ? v2[2] : max[2];

		}
		SetBoundingBox(min[0], min[1], min[2], max[0], max[1], max[2]);
	}
	void DrawableLine::UpdateGL()
	{
		std::vector<glm::vec3> vertices;
		vertices.reserve(m_Lines.size() * 2);
		uint32_t count = 0;
		std::vector<uint32_t> indices;
		indices.reserve(m_Lines.size()*2);
		for (const auto& line : m_Lines)
		{
			vertices.emplace_back(line.V1);
			vertices.emplace_back(line.V2);
			indices.emplace_back(count++);
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