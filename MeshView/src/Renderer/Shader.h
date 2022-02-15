#pragma once

#include<CoreModel/Macro.h>
#include<string>
#include<glm/glm.hpp>
namespace MV
{
	class Shader
	{
	public:
		Shader(const std::string& filePath);
		~Shader();
		void Bind() const;
		void UnBind() const;

		static Ref<Shader> Create(const std::string& filepath);



		void SetMat4(const std::string& name, const glm::mat4& value);
		void SetFloat(const std::string& name, float value);
		void SetFloat2(const std::string& name, const glm::vec2& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetInt1(const std::string& name, int value);
		void SetIntArray(const std::string& name, int* values, unsigned int count);
	private:
		std::pair<std::string, std::string> ReadFile(const std::string& filePath);
		
		//vertexshader:type=0; fragmentshader:type=1;
		uint32_t compileShader(const std::string& souceCode,int type);
	private:
		uint32_t m_ProgramId;
	};
}