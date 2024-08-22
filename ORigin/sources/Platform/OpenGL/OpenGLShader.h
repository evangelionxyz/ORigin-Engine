// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include "Origin/Renderer/Shader.h"

namespace origin
{
    class OpenGLShader : public Shader
    {
    public:
        OpenGLShader(const std::filesystem::path &filepath, bool recompile);
        OpenGLShader(const std::filesystem::path &filepath, bool isSpirv, bool recompile);
        ~OpenGLShader();

        void Enable() const override;
        void Disable() const override;
        void Reload() override;
        bool IsSPIRV() const { return m_IsSPIRV; }
        bool IsRecompile() const { return m_IsRecompile; }
        const std::filesystem::path &GetFilepath() const override;

        void SetBool(const std::string &name, bool boolean) override;
        void SetFloat(const std::string &name, float v0) override;
        void SetFloat(const std::string &name, float v0, float v1) override;
        void SetFloat(const std::string &name, float v0, float v1, float v2) override;
        void SetFloat(const std::string &name, float v0, float v1, float v2, float v3) override;
        void SetInt(const std::string &name, int v0) override;
        void SetInt(const std::string &name, int v0, int v1) override;
        void SetInt(const std::string &name, int v0, int v1, int v2) override;
        void SetInt(const std::string &name, int v0, int v1, int v2, int v3) override;
        void SetIntArray(const std::string &name, int *values, uint32_t count) override;
        void SetVector(const std::string &name, const glm::vec2 &vectors, int count) override;
        void SetVector(const std::string &name, const glm::vec3 &vectors, int count) override;
        void SetVector(const std::string &name, const glm::vec4 &vectors, int count) override;
        void SetMatrix(const std::string &name, const glm::mat2 &matrices, int count) override;
        void SetMatrix(const std::string &name, const glm::mat3 &matrices, int count) override;
        void SetMatrix(const std::string &name, const glm::mat4 &matrices, int count) override;
        int GetUniformLocation(const std::string &name) override;

    private:
        void SetUniformBool(const std::string &name, bool boolean);
        void SetUniformFloat(const std::string &name, float v0);
        void SetUniformFloat(const std::string &name, float v0, float v1);
        void SetUniformFloat(const std::string &name, float v0, float v1, float v2);
        void SetUniformFloat(const std::string &name, float v0, float v1, float v2, float v3);
        void SetUniformInt(const std::string &name, int v0);
        void SetUniformInt(const std::string &name, int v0, int v1);
        void SetUniformInt(const std::string &name, int v0, int v1, int v2);
        void SetUniformInt(const std::string &name, int v0, int v1, int v2, int v3);
        void SetUniformIntArray(const std::string &name, int *values, uint32_t count);
        void SetUniformVector(const std::string &name, const glm::vec2 &vectors, int count);
        void SetUniformVector(const std::string &name, const glm::vec3 &vectors, int count);
        void SetUniformVector(const std::string &name, const glm::vec4 &vectors, int count);
        void SetUniformMatrix(const std::string &name, const glm::mat2 &matrices, int count);
        void SetUniformMatrix(const std::string &name, const glm::mat3 &matrices, int count);
        void SetUniformMatrix(const std::string &name, const glm::mat4 &matrices, int count);

        ShaderProgramSources m_ShaderSource;
        std::filesystem::path m_Filepath;

        uint32_t m_RendererID;
        bool m_IsSPIRV = false;
        bool m_IsRecompile;

        ShaderData m_OpenGLSPIRV;
        ShaderSource m_OpenGLSourceCode;
        std::unordered_map<std::string, int> m_UniformLocationCache;

        void CreateSpirvProgram();
        uint32_t CompileShader(uint32_t type, const std::string &source);
        uint32_t CreateProgram(std::string vertexSrc, std::string fragmentSrc, std::string geometrySrc);
    };
}

#endif