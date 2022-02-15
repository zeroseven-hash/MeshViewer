#pragma once
#include<CoreModel/Macro.h>

namespace MV
{
	class IndexBuffer
	{
	public:
		IndexBuffer(const uint32_t* indices, uint32_t count);
		~IndexBuffer();

		void Bind();
		void UnBind();
		
		inline void SetCount(uint32_t count) { m_Count = count; }
		inline uint32_t GetCount()const { return m_Count; }

		static Ref<IndexBuffer> Create(const uint32_t* indices, uint32_t count);

	private:
		uint32_t m_RendererId;
		uint32_t m_Count;
	};
}