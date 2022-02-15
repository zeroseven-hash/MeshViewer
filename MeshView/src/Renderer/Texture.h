#pragma once
#include<CoreModel/Macro.h>

namespace MV
{
	class Texture
	{
	public:
		Texture() = default;
		Texture(const std::string& filepath);
		~Texture();

		uint32_t GetRendererId() { return m_RendererId; }
		void Bind();
		void UnBind();

	private:
		uint32_t m_RendererId;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
	};
}