#pragma once
#include<CoreModel/Layer.h>
#include<Renderer/Shader.h>
namespace MV
{
	class RenderPanel
	{
	public:

		RenderPanel() = default;
		RenderPanel(Layer* context):m_Context(context){}

		bool GetSSAOFlag()const { return m_SSAOFlag; }
		uint32_t GetSSAASample()const { return m_SSAASample; }

		void SetSAAOFlag(bool flag) { m_SSAOFlag = flag; }
		void SetSSAASample(uint32_t sample) { m_SSAASample = sample; }

		void SetLightShader(const Ref<Shader> lightShader) { m_LightShader = lightShader; }
		void SetContext(Layer* context) { m_Context = context; }
		void OnImGuiRender();

	private:
		Layer* m_Context;

		bool m_SSAOFlag = false;

		Ref<Shader> m_LightShader;
		uint32_t m_SSAASample = 2;
	};
}