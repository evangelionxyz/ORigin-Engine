// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include "Origin/Renderer/Shader.h"

#include <unordered_map>
#include <glad/glad.h>
#include <cstdint>
#include <vector>

namespace origin
{
	enum class ShaderType
	{
		NONE = -1,
		VERTEX,
		FRAGMENT,
		GEOMTERY
	  };

	struct ShaderProgramSources 
	{
		std::string VertexSources;
		std::string FragmentSources;
		std::string GeometrySources;
	};

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name, const std::string& filepath);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc);
		OpenGLShader(const std::string& filepath, bool enableSpirv, bool recompileSpirv = false);

		~OpenGLShader();

		void Enable() const override;
		void Disable() const override;

		void Reload() override;

		bool IsSpirvEnabled() const { return m_EnableSpirv; }
		bool IsRecompilerSpirv() const { return m_RecompileSPIRV; }

		const std::string& GetName() const override { return m_Name; }

		std::string ReadFile() const override
		{
			std::string source = (m_Filepath + "\n" +
			"\nVertex\n" + m_ShaderSource.VertexSources +
			"Fragment\n" + m_ShaderSource.FragmentSources); 
			return source; 
		}

		std::string GetFilepath() const override { return m_Filepath; }

		// BOOLEAN UNIFORM
		void SetBool(const std::string& name, bool boolean) override;

		// FLOAT UNIFORM
		void SetFloat(const std::string& name, float v0) override;
		void SetFloat(const std::string& name, float v0, float v1) override;
		void SetFloat(const std::string& name, float v0, float v1, float v2) override;
		void SetFloat(const std::string& name, float v0, float v1, float v2, float v3) override;

		// INT UNIFORM
		void SetInt(const std::string& name, int v0) override;
		void SetInt(const std::string& name, int v0, int v1) override;
		void SetInt(const std::string& name, int v0, int v1, int v2) override;
		void SetInt(const std::string& name, int v0, int v1, int v2, int v3) override;

		void SetIntArray(const std::string& name, int* values, uint32_t count) override;

		// VECTOR UNIFORM
		void SetVector(const std::string& name, const glm::vec2& vectors, int count)override;
		void SetVector(const std::string& name, const glm::vec3& vectors, int count) override;
		void SetVector(const std::string& name, const glm::vec4& vectors, int count) override;

		// MATRIX UNIFORM
		void SetMatrix(const std::string& name, const glm::mat2& matrices, int count) override;
		void SetMatrix(const std::string& name, const glm::mat3& matrices, int count) override;
		void SetMatrix(const std::string& name, const glm::mat4& matrices, int count) override;

		int GetUniformLocation(const std::string &name) override;

	private:
		// BOOLEAN UNIFORM
		void SetUniformBool(const std::string& name, bool boolean);

		// FLOAT UNIFORM
		void SetUniformFloat(const std::string& name, float v0);
		void SetUniformFloat(const std::string& name, float v0, float v1);
		void SetUniformFloat(const std::string& name, float v0, float v1, float v2);
		void SetUniformFloat(const std::string& name, float v0, float v1, float v2, float v3);

		// INT UNIFORM
		void SetUniformInt(const std::string& name, int v0);
		void SetUniformInt(const std::string& name, int v0, int v1);
		void SetUniformInt(const std::string& name, int v0, int v1, int v2);
		void SetUniformInt(const std::string& name, int v0, int v1, int v2, int v3);

		void SetUniformIntArray(const std::string& name, int* values, uint32_t count);

		// VECTOR UNIFORM
		void SetUniformVector(const std::string& name, const glm::vec2& vectors, int count);
		void SetUniformVector(const std::string& name, const glm::vec3& vectors, int count);
		void SetUniformVector(const std::string& name, const glm::vec4& vectors, int count);

		// MATRIX UNIFORM
		void SetUniformMatrix(const std::string& name, const glm::mat2& matrices, int count);
		void SetUniformMatrix(const std::string& name, const glm::mat3& matrices, int count);
		void SetUniformMatrix(const std::string& name, const glm::mat4& matrices, int count);

		std::string m_Filepath;
		uint32_t m_RendererID;
		bool m_EnableSpirv = false;
		bool m_RecompileSPIRV;

		std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
		std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;
		std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;

		std::unordered_map<std::string, int> m_UniformLocationCache;
		ShaderProgramSources m_ShaderSource;
		std::string m_Name;

		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		std::string ReadFile(const std::string& filepath);
		bool CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
		bool CompileOrGetOpenGLBinaries();
		void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);
		void CreateSpirvProgram();

		ShaderProgramSources ParseShader(const std::string& filePath);
		uint32_t CompileShader(uint32_t type, const std::string& source);
		uint32_t CreateProgram(std::string vertexSrc, std::string fragmentSrc, std::string geometrySrc);
	};
}

#endif