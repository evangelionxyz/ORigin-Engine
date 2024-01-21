// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "pch.h"
#include "Origin\Renderer\Shader.h"

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

  public:
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
		void SetVector(const std::string& name, const glm::vec2& vectors)override;
		void SetVector(const std::string& name, const glm::vec3& vectors) override;
		void SetVector(const std::string& name, const glm::vec4& vectors) override;

		// MATRIX UNIFORM
		void SetMatrix(const std::string& name, const glm::mat2& matrices) override;
		void SetMatrix(const std::string& name, const glm::mat3& matrices) override;
		void SetMatrix(const std::string& name, const glm::mat4& matrices) override;

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
		void SetUniformVector(const std::string& name, const glm::vec2& vectors);
    void SetUniformVector(const std::string& name, const glm::vec3& vectors);
    void SetUniformVector(const std::string& name, const glm::vec4& vectors);

    // MATRIX UNIFORM
    void SetUniformMatrix(const std::string& name, const glm::mat2& matrices);
    void SetUniformMatrix(const std::string& name, const glm::mat3& matrices);
    void SetUniformMatrix(const std::string& name, const glm::mat4& matrices);

  private:
    std::string m_Filepath;
    GLuint m_RendererID;
    bool m_EnableSpirv;
    bool m_RecompileSPIRV;

    std::unordered_map<GLenum, std::vector<uint32_t>> m_VulkanSPIRV;
    std::unordered_map<GLenum, std::vector<uint32_t>> m_OpenGLSPIRV;
    std::unordered_map<GLenum, std::string> m_OpenGLSourceCode;

    std::unordered_map<std::string, int> m_UniformLocationCache;
    ShaderProgramSources m_ShaderSource;
    std::string m_Name;

    std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
    std::string ReadFile(const std::string& filepath);
    void CompileOrGetVulkanBinaries(const std::unordered_map<GLenum, std::string>& shaderSources);
    void CompileOrGetOpenGLBinaries();
    void Reflect(GLenum stage, const std::vector<uint32_t>& shaderData);
    void CreateSpirvProgram();

    ShaderProgramSources ParseShader(const std::string& filePath);
    GLuint CompileShader(GLuint type, const std::string& source);
    GLuint CreateProgram(std::string vertexSrc, std::string fragmentSrc, std::string geometrySrc);
    int GetUniformLocation(const std::string& name);
  };
}