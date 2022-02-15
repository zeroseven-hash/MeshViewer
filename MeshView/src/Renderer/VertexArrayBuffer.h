#pragma once

#include"VertexBuffer.h"
#include"IndexBuffer.h"
namespace MV
{
	class VertexArrayBuffer
	{
	public:
		VertexArrayBuffer();
		~VertexArrayBuffer();

		void Bind() const;
		void UnBind() const;

		void SetVertexBuffer(Ref<VertexBuffer>& vb);
		void SetIndexBuffer(Ref<IndexBuffer>& ib);

		
		const Ref<IndexBuffer>& GetIndexBuffer()const { return m_IndexBuffer; }
		static Ref<VertexArrayBuffer> Create();
	private:
		uint32_t m_RendererId;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}