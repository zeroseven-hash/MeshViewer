#include "pch.h"
#include "Renderer.h"

#include<random>


#include<glad/glad.h>
namespace MV
{

	glm::mat4 Renderer::s_ProjectionMatrix = glm::mat4(1.0f);
	glm::mat4 Renderer::s_ViewMatrix = glm::mat4(1.0f);
	Ref<Shader> Renderer::s_GeometryPassShader = nullptr;
	Renderer::SSAOStuff Renderer::s_SSAOStuff = Renderer::SSAOStuff();
	
	Ref<Shader> Renderer::s_SSAAShader = nullptr;
	static unsigned int quadVAO = 0;
	static unsigned int quadVBO;


	
	void Renderer::RenderTexture()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = {
				// positions        // texture Coords
				-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
				 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
				 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			};
			// setup plane VAO
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

	
	void Renderer::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
	}

	void Renderer::Clear()
	{
		//TODO:add parma
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	}

	void Renderer::SetClearColor(float r, float g, float b, float a)
	{
		glClearColor(r, g, b, a);
	}

	void Renderer::Begin(const Camera& camera)
	{
		s_ProjectionMatrix = camera.GetProjection();
		s_ViewMatrix = camera.GetViewMatrix();
	}



	

	void Renderer::end()
	{

	}
	



	void Renderer::RenderMeshGbuffer(const Ref<DrawableMesh>& mesh, const glm::mat4& transform, StateFunc stateFunc)
	{
		if (!s_GeometryPassShader)
			s_GeometryPassShader=Shader::Create("assets/shaders/ssao_g_shader.glsl");

		DrawMesh(s_GeometryPassShader, mesh, transform, stateFunc);
	}

	void Renderer::RenderSSAO(const Ref<FrameBuffer>& ssaoFBO, const Ref<FrameBuffer>& ssaoBlurFBO, uint32_t gPostion, uint32_t gNormal)
	{
		if (!s_SSAOStuff.SSAOInit)
			s_SSAOStuff.initialize();
		//SSAO
		ssaoFBO->Bind();
		
		glClear(GL_COLOR_BUFFER_BIT);
		glm::vec2 noiseScale = glm::vec2((float)ssaoFBO->GetWidth() / 4.0f, (float)ssaoFBO->GetHeight() / 4.0f);
		s_SSAOStuff.SSAOShader->Bind();
		s_SSAOStuff.SSAOShader->SetMat4("projection", s_ProjectionMatrix);
		s_SSAOStuff.SSAOShader->SetFloat2("noiseScale", noiseScale);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPostion);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, s_SSAOStuff.NoiseTex);
		RenderTexture();
		ssaoFBO->UnBind();


		//Blur
		ssaoBlurFBO->Bind();
		glClear(GL_COLOR_BUFFER_BIT);
		s_SSAOStuff.SSAOBlurShader->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoFBO->GetColorAttachment());
		RenderTexture();
		ssaoBlurFBO->UnBind();

		
		
	}

	void Renderer::ResolveSSAABuffer(uint32_t ssaaTexture, uint32_t sslevel)
	{
		if (!s_SSAAShader)
		{
			s_SSAAShader = Shader::Create("assets/shaders/ssaa.glsl");
			s_SSAAShader->Bind();
			s_SSAAShader->SetInt1("SuperSampleTexture", 0);
		}

		s_SSAAShader->Bind();
		s_SSAAShader->SetInt1("SSlevel", sslevel);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaaTexture);
		RenderTexture();
	}

	void Renderer::DefferedShading(Ref<Shader>& shader, uint32_t gPosition, uint32_t gNormal, uint32_t gAlbedo, uint32_t gIndex,uint32_t ssao)
	{
		//shader unform Texture should be Set
		shader->Bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, ssao);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, gIndex);
		RenderTexture();
	}

	void Renderer::DrawMesh(Ref<Shader>& shader, const Ref<DrawableMesh>& mesh, const glm::mat4& transform,StateFunc stateFunc)
	{
		auto& vao = mesh->GetVertexArrayBuffer();
		vao->Bind();
		vao->GetIndexBuffer()->Bind();
		stateFunc();

		SetShaderMvp(shader, transform);
		switch (mesh->GetMeshType())
		{
		case MeshType::HEX:
		case MeshType::QUADMESH:
			glDrawElements(GL_QUADS, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr); break;
		case MeshType::TRIMESH:
		case MeshType::TET:
			glDrawElements(GL_TRIANGLES, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr); break;
		}
		glLineWidth(1.0f);
	}

	void Renderer::DrawLine(Ref<Shader>& shader, const Ref<VertexArrayBuffer>& vao, const glm::mat4& transform, StateFunc stateFunc)
	{
		
		SetShaderMvp(shader, transform);

		
		//Don't Forget!!!
		vao->Bind();
		vao->GetIndexBuffer()->Bind();

		stateFunc();
		glDrawElements(GL_LINES, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

		//reset the line width
		glLineWidth(1.0f);
	}


	void Renderer::DrawPoint(Ref<Shader>& shader, const Ref<VertexArrayBuffer>& vao, const glm::mat4& transform, StateFunc stateFunc)
	{
		SetShaderMvp(shader, transform);

		//Don't Forget!!!
		vao->Bind();
		vao->GetIndexBuffer()->Bind();
		stateFunc();


		glDrawElements(GL_POINTS, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);

		//reset the line width
		glPointSize(1.0f);

	}

	void Renderer::DrawArrow(Ref<Shader>& shader, const Ref<DrawableArrow>& arrow, const glm::mat4& transform, StateFunc stateFunc)
	{
		shader->Bind();
		glm::mat4 newTs = arrow->GetTranslate() *transform* arrow->GetRotation();
		SetShaderMvp(shader, newTs);
		shader->SetFloat3("u_Color", arrow->GetColor());
		
		auto vao = arrow->GetVertexArrayBuffer();
		vao->Bind();
		vao->GetIndexBuffer()->Bind();
		stateFunc();
		glDrawElements(GL_TRIANGLES, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
	
	void Renderer::SetShaderMvp(const Ref<Shader>& shader, const glm::mat4& transform)
	{
		shader->Bind();
		shader->SetMat4("u_ProjectMatrix", s_ProjectionMatrix);
		shader->SetMat4("u_ModelMatrix", transform);
		shader->SetMat4("u_ViewMatrix", s_ViewMatrix);
	}


	void Renderer::SSAOStuff::initialize()
	{
		SSAOShader = Shader::Create("assets/shaders/ssao.glsl");
		SSAOBlurShader = Shader::Create("assets/shaders/ssao_blur.glsl");


		SSAOShader->Bind();
		SSAOShader->SetInt1("gPosition", 0);
		SSAOShader->SetInt1("gNormal", 1);
		SSAOShader->SetInt1("texNoise", 2);
		SSAOShader->SetInt1("kernelSize", KernelSize);
		SSAOShader->SetFloat("radius", Radius);
		SSAOShader->SetFloat("bias", Bias);

		SSAOBlurShader->Bind();
		SSAOBlurShader->SetInt1("ssaoInput", 0);

		std::vector<glm::vec3> ssaoNoise;
		std::uniform_real_distribution<GLfloat> randomFloats(0.0f, 1.0f);
		std::default_random_engine generator;
		auto lerp = [](float a, float b, float f) {return a + f * (b - a); };

		SSAOKernel.clear();
		ssaoNoise.clear();

		//generate sample
		for (unsigned int i = 0; i < 64; i++)
		{
			glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
			sample = glm::normalize(sample);
			sample *= randomFloats(generator);
			float scale = float(i) / 64.0;

			// scale samples s.t. they're more aligned to center of kernel
			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			SSAOKernel.push_back(sample);
		}

		
		//generate noise texture
		for (unsigned int i = 0; i < 16; i++)
		{
			glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0f, randomFloats(generator) * 2.0 - 1.0f, 0.0f); // rotate around z-axis (in tangent space)
			ssaoNoise.push_back(noise);
		}
		glGenTextures(1, &NoiseTex);
		glBindTexture(GL_TEXTURE_2D, NoiseTex);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		SSAOShader->Bind();
		for (unsigned int i = 0; i < 64; ++i)
			SSAOShader->SetFloat3("samples[" + std::to_string(i) + "]",SSAOKernel[i]);
		SSAOInit = true;
	}
}

