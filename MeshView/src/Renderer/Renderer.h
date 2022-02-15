#pragma once

#include"Camera.h"
#include"Shader.h"
#include"VertexArrayBuffer.h"
#include"FrameBuffer.h"
#include"../DataStructure/DrawableMesh.h"
#include"../DataStructure/DrawableLine.h"
#include"../DataStructure/DrawablePoint.h"
#include"../DataStructure/DrawableArrow.h"

using StateFunc = std::function<void()>;
namespace MV
{
	
	class Renderer
	{
	public:
		struct SSAOStuff
		{
			bool SSAOInit=false;
			Ref<Shader>SSAOShader=nullptr;
			Ref<Shader>SSAOBlurShader=nullptr;

			uint32_t NoiseTex=0;
			std::vector<glm::vec3> SSAOKernel;

			SSAOStuff() = default;

			int KernelSize = 24;
			float Radius = 0.5;
			float Bias = 0.025f;
			void initialize();
		};
	public:
		
		static void Init();
		static void Clear();
		static void SetClearColor(float r, float g, float b, float a);
		static void Begin(const Camera& camera);
		static void end();
		
		
		
		static void RenderMeshGbuffer(const Ref<DrawableMesh>& mesh, const glm::mat4& transform, StateFunc stateFunc = StateFunc());
		static void RenderSSAO(const Ref<FrameBuffer>& ssaoFBO,const Ref<FrameBuffer>& ssaoBlurFBO,uint32_t gPostion, uint32_t gNormal);
		

		static void ResolveSSAABuffer(uint32_t ssaaTexture, uint32_t sslevel);
		static void DefferedShading(Ref<Shader>& shader,uint32_t gPosition,uint32_t gNormal,uint32_t gAlbedo,uint32_t gIndex,uint32_t ssao=0);

		static void DrawMesh(Ref<Shader>& shader, const Ref<DrawableMesh>& mesh, const glm::mat4& transform,StateFunc stateFunc = StateFunc());
		static void DrawLine(Ref<Shader>& shader, const Ref<VertexArrayBuffer>& vao, const glm::mat4& transform, StateFunc stateFunc= StateFunc());
		static void DrawPoint(Ref<Shader>& shader, const Ref<VertexArrayBuffer>& vao, const glm::mat4& transform, StateFunc stateFunc= StateFunc());
		static void DrawArrow(Ref<Shader>& shader, const Ref<DrawableArrow>& arrow, const glm::mat4& transform, StateFunc stateFunc = StateFunc());
		static void SetSSAOPatameter(int kernelSize, float radius, float bias)
		{
			s_SSAOStuff.KernelSize = kernelSize;
			s_SSAOStuff.Radius = radius;
			s_SSAOStuff.Bias = bias;
			if (s_SSAOStuff.SSAOInit)
			{
				s_SSAOStuff.SSAOShader->Bind();
				s_SSAOStuff.SSAOShader->SetInt1("kernelSize", kernelSize);
				s_SSAOStuff.SSAOShader->SetFloat("radius", radius);
				s_SSAOStuff.SSAOShader->SetFloat("bias", bias);
			}
		}
		static void RenderTexture();
		
		
	private:
		static void SetShaderMvp(const Ref<Shader>& shader, const glm::mat4& transform);
	

	private:
		static glm::mat4	s_ViewMatrix;
		static glm::mat4	s_ProjectionMatrix;
		static Ref<Shader>	s_GeometryPassShader;
		
	
		static Ref<Shader>	s_SSAAShader;
		//SSAO status
		static SSAOStuff	s_SSAOStuff;
	};





	
}