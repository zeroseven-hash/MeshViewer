#include "pch.h"
#include "FrameBuffer.h"

#include<glad/glad.h>
namespace MV
{
	namespace Utils
	{
		static GLenum TextureTarget(bool mutilSample)
		{
			return mutilSample ? GL_TEXTURE_2D_MULTISAMPLE: GL_TEXTURE_2D;
		}

		static GLenum TextureFilterToGL(TextureFilter filter)
		{
			switch (filter)
			{
			case TextureFilter::LINEAR: return GL_LINEAR;
			case TextureFilter::NEARST: return GL_NEAREST;
			}
		}
		static GLenum TextureWrapToGL(TextureWrap wrap)
		{
			switch (wrap)
			{
			case MV::TextureWrap::REPEAT: return GL_REPEAT;
			case MV::TextureWrap::MIRRORED_REPEAT:return GL_MIRRORED_REPEAT;
			case MV::TextureWrap::CLAMP_TO_EDGE:return GL_CLAMP_TO_EDGE;
			case MV::TextureWrap::CLAMP_TO_BORDER:return GL_CLAMP_TO_BORDER;
			}
		}
		static void AttachColorTexture(uint32_t id, int samples, TextureAttachmentSpecification spec, uint32_t width, uint32_t height, int index)
		{
			bool mutilsample = samples > 1;

			if (!mutilsample)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilterToGL(spec.MinFilter));
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilterToGL(spec.MagFilter));
				if (spec.WrapS != TextureWrap::Default && spec.WrapT != TextureWrap::Default)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrapToGL(spec.WrapS));
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrapToGL(spec.WrapT));
				}

			}

			switch (spec.Format)
			{
				case TextureFormat::RGBA:
				{
					if (mutilsample)
						glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE);
					else
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
					break;
				}
				case TextureFormat::RED:
				{
					if (mutilsample)
						glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE);
					else
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, nullptr);
					break;
				}
				case TextureFormat::RED_INTERGER:
				{
					if (mutilsample)
						glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE);
					else
						glTexImage2D(GL_TEXTURE_2D, 0, GL_R32I, width, height, 0, GL_RED_INTEGER, GL_INT, nullptr);
					break;
				}
			}


			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(mutilsample), id, 0);
		}
	}

	FrameBuffer::FrameBuffer(const FrameBufferSpecification& spec)
		:m_Specificaiton(spec)
	{
		Init();
	}

	
	FrameBuffer::~FrameBuffer()
	{
		Destroy();
	}
	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);
	}
	void FrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void FrameBuffer::Resize(uint32_t width, uint32_t height)
	{	
		m_Specificaiton.Width = width;
		m_Specificaiton.Height = height;
		Init();
	}

	Ref<FrameBuffer> FrameBuffer::Create(const FrameBufferSpecification& spec)
	{
		return std::make_shared<FrameBuffer>(spec);
	}

	void FrameBuffer::Init()
	{

		if (m_RendererId)
		{
			Destroy();
			m_ColorAttachments.clear();
			m_DepthAndStencilAttachment = 0;
		}

		glGenFramebuffers(1, &m_RendererId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);

		bool mutilsamples = m_Specificaiton.Samples > 1;

		if (m_Specificaiton.ColorAttachementSpecs.size())
		{
			m_ColorAttachments.resize(m_Specificaiton.ColorAttachementSpecs.size());
			glGenTextures(m_ColorAttachments.size(), m_ColorAttachments.data());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++)
			{
				glBindTexture(Utils::TextureTarget(mutilsamples), m_ColorAttachments[i]);
				Utils::AttachColorTexture(m_ColorAttachments[i], m_Specificaiton.Samples, m_Specificaiton.ColorAttachementSpecs[i], m_Specificaiton.Width, m_Specificaiton.Height, i);
			}
		}

		if (m_Specificaiton.DepthAttachmentSpec.Format != TextureFormat::NONE)
		{
			glGenRenderbuffers(1, &m_DepthAndStencilAttachment);
			glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAndStencilAttachment);
			if (mutilsamples)
				glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Specificaiton.Samples, GL_DEPTH24_STENCIL8, m_Specificaiton.Width,m_Specificaiton.Height);
			else
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Specificaiton.Width, m_Specificaiton.Height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAndStencilAttachment);
		}
		
		if (m_ColorAttachments.size() >= 1)
		{
			IGAME_ASSERT(m_ColorAttachments.size() <= 4, "");
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2 ,GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(m_ColorAttachments.size(), buffers);
		}
		else
		{
			glDrawBuffer(GL_NONE);
		}
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			MESH_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	void FrameBuffer::Destroy()
	{
		glDeleteFramebuffers(1, &m_RendererId);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteRenderbuffers(1, &m_DepthAndStencilAttachment);
	}

	

	void FrameBuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		IGAME_ASSERT(attachmentIndex < m_ColorAttachments.size(), " ");
		auto& spec = m_Specificaiton.ColorAttachementSpecs[attachmentIndex];

		switch (spec.Format)
		{
		case TextureFormat::RED:
			glClearTexImage(m_ColorAttachments[attachmentIndex], 0, GL_RED, GL_FLOAT, &value); break;
		case TextureFormat::RED_INTERGER:
			glClearTexImage(m_ColorAttachments[attachmentIndex], 0, GL_RED_INTEGER, GL_INT, &value); break;
		}

		
	}

	int FrameBuffer::ReadPixelI(uint32_t attachmentIndex, int x, int y)
	{
		IGAME_ASSERT(attachmentIndex < m_ColorAttachments.size(), " ");
		Bind();
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		
		UnBind();
		return pixelData;
	}

	




	//MultiSampleFrameBuffer

	/*MultiSampleFrameBuffer::MultiSampleFrameBuffer(uint32_t width, uint32_t height)
		:m_IntermediateFbo(width,height),
		m_Width(width),m_Height(height)
	{
		Init(width,height);
	}

	MultiSampleFrameBuffer::~MultiSampleFrameBuffer()
	{
		Destroy();
	}

	void MultiSampleFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);
	}

	void MultiSampleFrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void MultiSampleFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		m_IntermediateFbo.Resize(width, height);
		Destroy();
		Init(width, height);
	}

	Ref<MultiSampleFrameBuffer> MultiSampleFrameBuffer::Create(uint32_t width, uint32_t height)
	{
		return std::make_shared<MultiSampleFrameBuffer>(width, height);
	}

	void MultiSampleFrameBuffer::Blit()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_RendererId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_IntermediateFbo.m_RendererId);
		glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, m_Width, m_Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void MultiSampleFrameBuffer::Init(uint32_t width, uint32_t height)
	{
		glGenFramebuffers(1, &m_RendererId);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);

		glGenTextures(1, &m_MultiSampleTextureAttachment);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MultiSampleTextureAttachment);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_MultiSampleTextureAttachment, 0);

		glGenRenderbuffers(1, &m_DepthAndStencilAttachment);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAndStencilAttachment);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAndStencilAttachment);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			MESH_ERROR("ERROR::FRAMEBUFFER:: MultiSampleFramebuffer is not complete!");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void MultiSampleFrameBuffer::Destroy()
	{
		glDeleteFramebuffers(1, &m_RendererId);
		glDeleteTextures(1, &m_MultiSampleTextureAttachment);
		glDeleteRenderbuffers(1, &m_DepthAndStencilAttachment);
	}*/
}
