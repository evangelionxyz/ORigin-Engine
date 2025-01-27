// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include "Origin/Renderer/Shader.h"

namespace origin
{
    class OpenGLShader final : public Shader
    {
    public:
        OpenGLShader(const std::filesystem::path &filepath, bool recompile);
        OpenGLShader(const std::filesystem::path &filepath, bool isSpirv, bool recompile);
        ~OpenGLShader() override;

        void Enable() const override;
        void Disable() const override;
        void Reload() override;
        bool IsSPIRV() const override { return m_IsSPIRV; }
        bool IsRecompile() const override { return m_IsRecompile; }
        const std::filesystem::path &GetFilepath() const override;

        void SetBool(const std::string &name, bool boolean) override;

        void SetFloat(const std::string &name, float v0) override;
        void SetFloat(const std::string &name, float v0, float v1) override;
        void SetFloat(const std::string &name, float v0, float v1, float v2) override;
        void SetFloat(const std::string &name, float v0, float v1, float v2, float v3) override;
        void SetFloat(const std::string &name, f32 *values, i32 count);

        void SetInt(const std::string &name, i32 v0) override;
        void SetInt(const std::string &name, i32 v0, i32 v1) override;
        void SetInt(const std::string &name, i32 v0, i32 v1, i32 v2) override;
        void SetInt(const std::string &name, i32 v0, i32 v1, i32 v2, i32 v3) override;
        void SetInt(const std::string &name, i32 *values, i32 count) override;

        void SetVector(const std::string &name, const glm::vec2 &vectors, i32 count) override;
        void SetVector(const std::string &name, const glm::vec3 &vectors, i32 count) override;
        void SetVector(const std::string &name, const glm::vec4 &vectors, i32 count) override;

        void SetMatrix(const std::string &name, const glm::mat2 &matrices, i32 count) override;
        void SetMatrix(const std::string &name, const glm::mat3 &matrices, i32 count) override;
        void SetMatrix(const std::string &name, const glm::mat4 &matrices, i32 count) override;
        i32 GetUniformLocation(const std::string &name) override;

    private:
        void SetUniformBool(const std::string &name, bool boolean);
        void SetUniformFloat(const std::string &name, float v0);
        void SetUniformFloat(const std::string &name, float v0, float v1);
        void SetUniformFloat(const std::string &name, float v0, float v1, float v2);
        void SetUniformFloat(const std::string &name, float v0, float v1, float v2, float v3);
        void SetUniformFloat(const std::string &name, f32 *values, i32 count);

        void SetUniformInt(const std::string &name, i32 v0);
        void SetUniformInt(const std::string &name, i32 v0, i32 v1);
        void SetUniformInt(const std::string &name, i32 v0, i32 v1, i32 v2);
        void SetUniformInt(const std::string &name, i32 v0, i32 v1, i32 v2, i32 v3);
        void SetUniformInt(const std::string &name, i32 *values, i32 count);

        void SetUniformVector(const std::string &name, const glm::vec2 &vectors, i32 count);
        void SetUniformVector(const std::string &name, const glm::vec3 &vectors, i32 count);
        void SetUniformVector(const std::string &name, const glm::vec4 &vectors, i32 count);
        void SetUniformMatrix(const std::string &name, const glm::mat2 &matrices, i32 count);
        void SetUniformMatrix(const std::string &name, const glm::mat3 &matrices, i32 count);
        void SetUniformMatrix(const std::string &name, const glm::mat4 &matrices, i32 count);

        ShaderProgramSources m_ShaderSource;
        std::filesystem::path m_Filepath;

        u32 m_RendererID;
        bool m_IsSPIRV = false;
        bool m_IsRecompile;

        ShaderData m_OpenGLSPIRV;
        ShaderSource m_OpenGLSourceCode;
        std::unordered_map<std::string, int> m_UniformLocationCache;

        void CreateSpirvProgram();

        u32 CompileShader(u32 type, const std::string &source);
        u32 CreateProgram(const std::string &vertex_source, const std::string &fragment_source,
            const std::string &geometry_source);
    };
}

#endif