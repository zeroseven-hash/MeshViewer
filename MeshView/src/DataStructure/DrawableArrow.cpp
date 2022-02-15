#include"pch.h"
#include"DrawableArrow.h"

#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtx/quaternion.hpp>
namespace MV
{
	DrawableArrow::DrawableArrow(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& color, float length, float thickness)
		:m_Pos(pos), m_Direction(dir), m_Length(length), m_Thickness(thickness), m_Color(color) 
	{
		Build();
		CalculateRotation();
		m_Translate = glm::translate(m_Translate, pos);

	}

	DrawableArrow::DrawableArrow(glm::vec3&& pos, glm::vec3&& dir, glm::vec3&& color, float length, float thickness)
		: m_Pos(std::move(pos)), m_Direction(std::move(dir)), m_Length(length), m_Color(color), m_Thickness(thickness) 
	{
		Build();
		CalculateRotation();
		m_Translate = glm::translate(m_Translate, pos);
	}
		

	void DrawableArrow::Build()
	{
		std::vector<glm::vec3> vertices;
		std::vector<uint32_t> indices;
		
		CalculateVertex(vertices, indices);
		Ref<VertexBuffer> vb = VertexBuffer::Create(vertices.data(), (uint32_t)(vertices.size() * sizeof(glm::vec3)));
		BufferLayout layout =
		{
			{"a_Position",OpenGLDataType::Float,3}
		};
		vb->SetLayout(layout);
		Ref<IndexBuffer> ib = IndexBuffer::Create(indices.data(), (uint32_t)indices.size());

		m_VertexArrayBuffer->SetVertexBuffer(vb);
		m_VertexArrayBuffer->SetIndexBuffer(ib);
	}

	

	void DrawableArrow::CalculateVertex(std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices)
	{
		glm::vec3 origin = { 0.0f,0.0f,0.0f };
		glm::vec3 dir = { 0.0f,0.0f,1.0f };
		float cylenderLen = m_Length * 0.77;
		float coneLen = m_Length * 0.23;
		int fragment = 10 + m_Thickness / 0.1;
		float radius = m_Thickness / 2;
		vertices.emplace_back(origin);
		float angle = 0;
		float delta = 2 * glm::pi<float>() / fragment;
		for (int i = 0; i < fragment; i++)
		{
			float x = radius * std::sin(angle);
			float y = radius * std::cos(angle);

			vertices.emplace_back(x,y, 0.0f);
			angle += delta;
			if (i == 0) continue;
			indices.emplace_back(i);
			indices.emplace_back(i+1);
			indices.emplace_back(0);
		}

		indices.emplace_back(fragment);
		indices.emplace_back(1);
		indices.emplace_back(0);

		int offset = fragment + 1;
		vertices.emplace_back(origin + dir * cylenderLen);
		int o = vertices.size() - 1;
		for (int i = 1; i <= fragment; i++)
		{
			size_t n = vertices.size();
			vertices.emplace_back(vertices[i].x, vertices[i].y, cylenderLen);
			if (n-1 == o) continue;
			indices.emplace_back(n-1);
			indices.emplace_back(n);
			indices.emplace_back(o);

			
			int p1 = i;
			int p2 = (i == fragment) ? 1 : i + 1;

			int p3 = p1 + offset;
			int p4 = p2 + offset;
			indices.emplace_back(p1);
			indices.emplace_back(p4);
			indices.emplace_back(p3);

			indices.emplace_back(p1);
			indices.emplace_back(p2);
			indices.emplace_back(p4);
		}
		indices.emplace_back(vertices.size() - 1);
		indices.emplace_back(o+1);
		indices.emplace_back(o);

		
		//circular cone
		glm::vec3 vert = vertices[o] + dir * coneLen;
		vertices.emplace_back(vert);
		size_t vert_id = vertices.size() - 1;
		for (int i = 1; i <= fragment; i++)
		{
			int p = i + offset;
			size_t n = vertices.size();
			vertices.emplace_back(vertices[p].x * 4.0f, vertices[p].y * 4.0f, vertices[p].z);
			if (i == 1)continue;
			indices.emplace_back(n-1);
			indices.emplace_back(n);
			indices.emplace_back(o);

			indices.emplace_back(n - 1);
			indices.emplace_back(n);
			indices.emplace_back(vert_id);



		}
		indices.emplace_back(vertices.size() - 1);
		indices.emplace_back(offset*2+1);
		indices.emplace_back(o);

		indices.emplace_back(vertices.size() - 1);
		indices.emplace_back(offset * 2 + 1);
		indices.emplace_back(vert_id);


	}
	void DrawableArrow::CalculateRotation()
	{
		auto& orgin = glm::vec3{ 0.0f,0.0f,1.0f };
		auto& target = m_Direction;
		glm::vec3 axis = glm::cross(orgin, target);
		
		double costheta = glm::dot(orgin, target) / (glm::length(orgin) * glm::length(target));
		double theta = -acos(costheta);


		float x = axis[0] * sin(theta / 2);
		float y = axis[1] * sin(theta / 2);
		float z = axis[2] * sin(theta / 2);
		float w = cos(theta / 2);
		m_Rotation = glm::inverse(glm::toMat4(glm::quat(w, x, y, z)));
		
	}
}

