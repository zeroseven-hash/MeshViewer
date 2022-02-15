#pragma once

#include<CoreModel/Macro.h>
#include<CoreModel/Log.h>
namespace MV
{
	enum class TextureFormat
	{
		NONE=0,
		//Color
		RGBA,
		RED_INTERGER,
		RED,

		//depth
		DEPTHANDSTENCIL
		
	};
	enum class TextureFilter
 	{
		LINEAR,NEARST
	};
	enum class TextureWrap
	{
		
		Default=0,REPEAT, MIRRORED_REPEAT,CLAMP_TO_EDGE,CLAMP_TO_BORDER
	};
	struct TextureAttachmentSpecification
	{
		TextureAttachmentSpecification() = default;
		TextureAttachmentSpecification(
			TextureFormat format,
			TextureFilter magFilter=TextureFilter::LINEAR, 
			TextureFilter minFilter=TextureFilter::LINEAR,
			TextureWrap wras=TextureWrap::Default,
			TextureWrap wrat=TextureWrap::Default
		)
			:Format(format),MagFilter(magFilter),MinFilter(minFilter), WrapS(wras),WrapT(wrat) {}

		
		TextureFormat Format;
		TextureFilter MagFilter;
		TextureFilter MinFilter;
		TextureWrap WrapS;
		TextureWrap WrapT;
	};

	struct FrameBufferAttachmentSpecification
	{
		FrameBufferAttachmentSpecification() = default;
		FrameBufferAttachmentSpecification(std::initializer_list<TextureAttachmentSpecification> attachmentSpecs)
			:AttachmentSpecs(attachmentSpecs){}
		std::vector<TextureAttachmentSpecification> AttachmentSpecs;
	};

	struct FrameBufferSpecification
	{
		FrameBufferSpecification(uint32_t width, uint32_t height, const FrameBufferAttachmentSpecification& attchmentSpec, uint32_t samples)
			:Width(width), Height(height), Samples(samples)
		{
			
			for (auto& format : attchmentSpec.AttachmentSpecs)
			{
				if (format.Format == TextureFormat::DEPTHANDSTENCIL)
					DepthAttachmentSpec=format;
				else
					ColorAttachementSpecs.push_back(format);
			}
		}
		uint32_t Width,Height;
		std::vector<TextureAttachmentSpecification> ColorAttachementSpecs;
		TextureAttachmentSpecification DepthAttachmentSpec=TextureAttachmentSpecification(TextureFormat::NONE);
		uint32_t Samples;
	};

	class FrameBuffer
	{
	public:
		
		FrameBuffer() = default;
		FrameBuffer(const FrameBufferSpecification& spec);
		~FrameBuffer();

		void Bind();
		void UnBind();

		void ClearAttachment(uint32_t attachmentIndex, int value);
		int ReadPixelI(uint32_t attachmentIndex, int x, int y);
		void Resize(uint32_t width, uint32_t height);
		inline uint32_t GetWidth()const { return m_Specificaiton.Width; }
		inline uint32_t GetHeight()const { return m_Specificaiton.Height; }

		inline uint32_t GetColorAttachment(uint32_t index = 0)const { IGAME_ASSERT(index < m_ColorAttachments.size(), "index out of range"); return m_ColorAttachments[index]; }

		inline uint32_t GetRenderedId()const { return m_RendererId; }
		static Ref<FrameBuffer> Create(const FrameBufferSpecification& spec);
	private:
		void Init();
		void Destroy();
	private:
		uint32_t m_RendererId=0;
		FrameBufferSpecification m_Specificaiton;
		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAndStencilAttachment=0;

		
		
	};

	/*class MultiSampleFrameBuffer
	{
	public:
		MultiSampleFrameBuffer(uint32_t width, uint32_t height);
		~MultiSampleFrameBuffer();

		void Bind();
		void UnBind();

		void Resize(uint32_t width, uint32_t height);
		
		inline uint32_t GetIntermediateTexture()const { return m_IntermediateFbo.m_textureAttachment; }
		static Ref<MultiSampleFrameBuffer> Create(uint32_t width, uint32_t height);

		void Blit();
	private:
		void Init(uint32_t width, uint32_t height);
		void Destroy();
	private:
		FrameBuffer m_IntermediateFbo;

		uint32_t m_RendererId;
		uint32_t m_MultiSampleTextureAttachment;
		uint32_t m_DepthAndStencilAttachment;
		uint32_t m_Width, m_Height;
	};*/
}