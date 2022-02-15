#include"pch.h"

#include "Texture.h"

#include<glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
namespace MV
{

	Texture::Texture(const std::string& filepath)
	{
		glGenTextures(1, &m_RendererId);

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
		m_Width = width;
		m_Height = height;
		m_Channels = channels;

		

		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &m_RendererId);
	}

	void Texture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D,m_RendererId);
	}

	void Texture::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}
