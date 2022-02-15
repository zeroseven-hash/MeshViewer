#include"pch.h"
#include"VertexBuffer.h"

#include<glad/glad.h>
namespace MV
{
	VertexBuffer::VertexBuffer(const void* data, uint32_t size)
	{
		glGenBuffers(1, &m_RendererId);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);

		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}


	VertexBuffer::~VertexBuffer()
	{
		glDeleteBuffers(1, &m_RendererId);
	}
	void VertexBuffer::Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	}
	void VertexBuffer::UnBind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	Ref<VertexBuffer> VertexBuffer::Create(const void* data, uint32_t size)
	{

		return std::make_shared<VertexBuffer>(data,size);
	}
}