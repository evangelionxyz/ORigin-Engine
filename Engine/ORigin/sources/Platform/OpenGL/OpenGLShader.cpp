// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Origin/Core/ConsoleManager.h"
#include "OpenGLShader.h"
#include "Origin/Core/Time.h"
#include "Origin/Core/Assert.h"
#include "Origin/Profiler/Profiler.h"

#include <glad/glad.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace origin
{
    OpenGLShader::OpenGLShader(const std::filesystem::path &filepath, const bool recompile)
        : m_Filepath(filepath), m_RendererID(0), m_IsSPIRV(true), m_IsRecompile(recompile)
    {
        PUSH_CONSOLE_INFO("[Shader] Load {}", m_Filepath.generic_string());
        OGN_CORE_TRACE("[Shader] Load {}", m_Filepath.generic_string());

        ShaderUtils::CreateCachedDirectoryIfNeeded();
        {
            Timer timer;
            const std::string source          = ReadFile(filepath.generic_string());
            const ShaderSource shader_sources = PreProcess(source, filepath.generic_string());
            const ShaderData vulkan_spirv     = CompileOrGetVulkanBinaries(shader_sources, filepath.generic_string());

            m_OpenGLSPIRV = CompileOrGetOpenGLBinaries(m_OpenGLSourceCode, vulkan_spirv, filepath.generic_string());

            CreateSpirvProgram();

            PUSH_CONSOLE_INFO("[Shader] Shader create took {} ms", timer.ElapsedMillis());
            OGN_CORE_TRACE("[Shader] Shader create took {} ms", timer.ElapsedMillis());
        }
    }

    OpenGLShader::OpenGLShader(const std::filesystem::path &filepath, const bool spirv, const bool recompile)
        : m_Filepath(filepath), m_RendererID(0), m_IsSPIRV(spirv), m_IsRecompile(recompile)
    {
        OGN_PROFILER_RENDERING();
        PUSH_CONSOLE_INFO("[Shader] Load {}", m_Filepath.generic_string());
        OGN_CORE_TRACE("[Shader] Load {}", m_Filepath.generic_string());

        // With SPIRV
        if (spirv)
        {
            Timer timer;
            ShaderUtils::CreateCachedDirectoryIfNeeded();

            const std::string source          = ReadFile(filepath.generic_string());
            const ShaderSource shader_sources = PreProcess(source, filepath.generic_string());
            const ShaderData vulkanSpirv      = CompileOrGetVulkanBinaries(shader_sources, filepath.generic_string());

            m_OpenGLSPIRV = CompileOrGetOpenGLBinaries(m_OpenGLSourceCode, vulkanSpirv, filepath.generic_string());

            CreateSpirvProgram();
            PUSH_CONSOLE_INFO("[Shader] Shader Creation took {} ms", timer.ElapsedMillis());
            OGN_CORE_TRACE("[Shader] Shader Creation took {} ms", timer.ElapsedMillis());
        }
        else // Without SPIRV
        {
            m_ShaderSource = ParseShader(filepath.string());
            m_RendererID   = CreateProgram(m_ShaderSource.VertexSources, m_ShaderSource.FragmentSources, m_ShaderSource.GeometrySources);
        }
    }

    OpenGLShader::~OpenGLShader()
    {
        OGN_PROFILER_RENDERING();
        glDeleteProgram(m_RendererID);
    }

    const std::filesystem::path &OpenGLShader::GetFilepath() const
    {
        return m_Filepath;
    }

    void OpenGLShader::CreateSpirvProgram()
    {
        OGN_PROFILER_RENDERING();

        const u32 program = glCreateProgram();
        std::vector<u32> shader_ids;

        for (auto &&[stage, spirv] : m_OpenGLSPIRV)
        {
            u32 shader_id = shader_ids.emplace_back(glCreateShader(stage));

            glShaderBinary(1, &shader_id, GL_SHADER_BINARY_FORMAT_SPIR_V,
                spirv.data(), static_cast<GLsizei>(spirv.size() * sizeof(u32)));

            glSpecializeShader(shader_id, "main", 0, nullptr, nullptr);
            glAttachShader(program, shader_id);
        }

        glLinkProgram(program);
        glValidateProgram(program);

        i32 is_linked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &is_linked);

        if (is_linked < 0)
        {
            OGN_CORE_ERROR("[Shader] Failed to linking shader");
            PUSH_CONSOLE_ERROR("[Shader] Failed to linking shader");
            return;
        }

        if (is_linked == GL_FALSE)
        {
            const i32 max_length = 512;
            char info_log[max_length];
            glGetProgramInfoLog(program, max_length, nullptr, reinterpret_cast<GLchar*>(info_log));
            glDeleteProgram(program);

            for (const auto id : shader_ids)
                glDeleteShader(id);

            return;
        }

        for (const auto id : shader_ids)
            glDeleteShader(id);

        m_RendererID = program;
    }

    u32 OpenGLShader::CompileShader(const u32 type, const std::string &source)
    {
        OGN_PROFILER_RENDERING();

        const u32 shader_id = glCreateShader(type);

        const char *src = source.c_str();

        glShaderSource(shader_id, 1, &src, nullptr);
        glCompileShader(shader_id);


        i32 success;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            i32 max_length;
            glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);

            char *info_log = new char[max_length];
            glGetShaderInfoLog(shader_id, max_length, &max_length, info_log);

            const char *m = "[Shader] Failed to Compile ";
            const char *shaderType{};

            switch (type)
            {
            case GL_VERTEX_SHADER:
                shaderType = "VERTEX Shader";
                break;
            case GL_FRAGMENT_SHADER:
                shaderType = "FRAGMENT Shader";
                break;
            case GL_GEOMETRY_SHADER:
                shaderType = "GEOMETRY Shader";
                break;
            }

            const size_t len = strlen(m) + strlen(shaderType) + 1;

            if (const auto msg = static_cast<char*>(malloc(len)); msg != nullptr)
            {
                strcpy(msg, m);
                strcat(msg, shaderType);

                OGN_CORE_ERROR("[Shader]: {}", msg);
                PUSH_CONSOLE_ERROR("[Shader]: {}", msg);

                free(msg);
            }

            PUSH_CONSOLE_ERROR("[Shader] {}", info_log);
            OGN_CORE_ASSERT(false, "[Shader] {}", info_log);

            delete[] info_log;
            return 0;
        }

        switch (type)
        {
        case GL_VERTEX_SHADER:
            OGN_CORE_INFO("[Shader] VERTEX shader compiled");
            PUSH_CONSOLE_INFO("[Shader] VERTEX shader compiled");
            break;
        case GL_FRAGMENT_SHADER:
            OGN_CORE_INFO("[Shader] FRAGMENT shader compiled");
            PUSH_CONSOLE_INFO("[Shader] FRAGMENT shader compiled");
            break;
        case GL_GEOMETRY_SHADER:
            OGN_CORE_INFO("[Shader] GEOMETRY shader compiled");
            PUSH_CONSOLE_INFO("[Shader] GEOMETRY shader compiled");
            break;
        }

        return shader_id;
    }

    u32 OpenGLShader::CreateProgram(const std::string& vertex_source, const std::string &fragment_source,
        const std::string &geometry_source)
    {
        OGN_PROFILER_RENDERING();

        // Create Program
        const u32 shader_program  = glCreateProgram();
        const u32 vertex_shader   = CompileShader(GL_VERTEX_SHADER, vertex_source);
        const u32 fragment_shader = CompileShader(GL_FRAGMENT_SHADER, fragment_source);

        if (!geometry_source.empty())
        {
            const u32 geometry_shader = CompileShader(GL_GEOMETRY_SHADER, geometry_source);
            glAttachShader(shader_program, geometry_shader);
        }

        // Attach and Link Shader->Program
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        glValidateProgram(shader_program);

        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        return shader_program;
    }

    void OpenGLShader::Enable() const
    {
        glUseProgram(m_RendererID);
    }
    void OpenGLShader::Disable() const
    {
        glUseProgram(0);
    }

    void OpenGLShader::Reload()
    {
        // With SPIRV
        if (m_IsSPIRV)
        {
            ShaderUtils::CreateCachedDirectoryIfNeeded();
            m_IsRecompile = true;
            {
                Timer timer;
                const std::string source          = ReadFile(m_Filepath.generic_string());
                const ShaderSource shader_sources = PreProcess(source, m_Filepath.generic_string());
                const ShaderData vulkan_spirv     = CompileOrGetVulkanBinaries(shader_sources, m_Filepath.generic_string());

                m_OpenGLSPIRV = CompileOrGetOpenGLBinaries(m_OpenGLSourceCode, vulkan_spirv, m_Filepath.generic_string());
                CreateSpirvProgram();
                OGN_CORE_TRACE("[Shader] Shader creation took {0} ms", timer.ElapsedMillis());
                PUSH_CONSOLE_INFO("[Shader] Shader creation took {0} ms", timer.ElapsedMillis());
            }

            m_IsRecompile = false;
        }
        else
        {
            glDeleteProgram(m_RendererID);

            m_ShaderSource = ParseShader(m_Filepath.string());
            m_RendererID   = CreateProgram(m_ShaderSource.VertexSources, m_ShaderSource.FragmentSources, m_ShaderSource.GeometrySources);
        }
    }

    void OpenGLShader::SetBool(const std::string &name, const bool boolean)
    {
        SetUniformBool(name, boolean);
    }

    void OpenGLShader::SetFloat(const std::string &name, const f32 v0)
    {
        SetUniformFloat(name, v0);
    }

    void OpenGLShader::SetFloat(const std::string &name, const f32 v0, const f32 v1)
    {
        SetUniformFloat(name, v0, v1);
    }

    void OpenGLShader::SetFloat(const std::string &name, const f32 v0, const f32 v1, const f32 v2)
    {
        SetUniformFloat(name, v0, v1, v2);
    }

    void OpenGLShader::SetFloat(const std::string &name, const f32 v0, const f32 v1, const f32 v2, const f32 v3)
    {
        SetUniformFloat(name, v0, v1, v2, v3);
    }

    void OpenGLShader::SetInt(const std::string &name, const i32 v0)
    {
        SetUniformInt(name, v0);
    }

    void OpenGLShader::SetInt(const std::string &name, const i32 v0, const i32 v1)
    {
        SetUniformInt(name, v0, v1);
    }

    void OpenGLShader::SetInt(const std::string &name, const i32 v0, const i32 v1, const i32 v2)
    {
        SetUniformInt(name, v0, v1, v2);
    }

    void OpenGLShader::SetInt(const std::string &name, const i32 v0, const i32 v1, const i32 v2, const i32 v3)
    {
        SetUniformInt(name, v0, v1, v2, v3);
    }

    void OpenGLShader::SetIntArray(const std::string &name, i32 *values, const i32 count)
    {
        SetUniformIntArray(name, values, count);
    }

    void OpenGLShader::SetVector(const std::string &name, const glm::vec2 &vectors, const i32 count)
    {
        SetUniformVector(name, vectors, count);
    }

    void OpenGLShader::SetVector(const std::string &name, const glm::vec3 &vectors, const i32 count)
    {
        SetUniformVector(name, vectors, count);
    }

    void OpenGLShader::SetVector(const std::string &name, const glm::vec4 &vectors, const i32 count)
    {
        SetUniformVector(name, vectors, count);
    }

    void OpenGLShader::SetMatrix(const std::string &name, const glm::mat2 &matrices, const i32 count)
    {
        SetUniformMatrix(name, matrices, count);
    }

    void OpenGLShader::SetMatrix(const std::string &name, const glm::mat3 &matrices, const i32 count)
    {
        SetUniformMatrix(name, matrices, count);
    }

    void OpenGLShader::SetMatrix(const std::string &name, const glm::mat4 &matrices, const i32 count)
    {
        SetUniformMatrix(name, matrices, count);
    }

    // private
    // BOOLEAN UNIFORM
    void OpenGLShader::SetUniformBool(const std::string &name, const bool boolean)
    {
        glUniform1i(GetUniformLocation(name), boolean);
    }

    // FLOAT UNIFORM
    void OpenGLShader::SetUniformFloat(const std::string &name, const f32 v0)
    {
        glUniform1f(GetUniformLocation(name), v0);
    }
    void OpenGLShader::SetUniformFloat(const std::string &name, const f32 v0, const f32 v1)
    {
        glUniform2f(GetUniformLocation(name), v0, v1);
    }
    void OpenGLShader::SetUniformFloat(const std::string &name, const f32 v0, const f32 v1, const f32 v2)
    {
        glUniform3f(GetUniformLocation(name), v0, v1, v2);
    }
    void OpenGLShader::SetUniformFloat(const std::string &name, const f32 v0, const f32 v1, const f32 v2, const f32 v3)
    {
        glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
    }
    // INT UNIFORM
    void OpenGLShader::SetUniformInt(const std::string &name, const i32 v0)
    {
        glUniform1i(GetUniformLocation(name), v0);
    }

    void OpenGLShader::SetUniformInt(const std::string &name, const i32 v0, const i32 v1)
    {
        glUniform2i(GetUniformLocation(name), v0, v1);
    }

    void OpenGLShader::SetUniformInt(const std::string &name,const i32 v0, const i32 v1,const i32 v2)
    {
        glUniform3i(GetUniformLocation(name), v0, v1, v2);
    }

    void OpenGLShader::SetUniformInt(const std::string &name, const i32 v0, const i32 v1, const i32 v2, const i32 v3)
    {
        glUniform4i(GetUniformLocation(name), v0, v1, v2, v3);
    }

    void OpenGLShader::SetUniformIntArray(const std::string &name, i32 *values, const i32 count)
    {
        glUniform1iv(GetUniformLocation(name), count, values);
    }

    // VECTOR UNIFORM
    void OpenGLShader::SetUniformVector(const std::string &name, const glm::vec2 &vectors, const i32 count)
    {
        glUniform2fv(GetUniformLocation(name), count, glm::value_ptr(vectors));
    }

    void OpenGLShader::SetUniformVector(const std::string &name, const glm::vec3 &vectors, const i32 count)
    {
        glUniform3fv(GetUniformLocation(name), count, glm::value_ptr(vectors));
    }

    void OpenGLShader::SetUniformVector(const std::string &name, const glm::vec4 &vectors, const i32 count)
    {
        glUniform4fv(GetUniformLocation(name), count, glm::value_ptr(vectors));
    }

    // MATRIX UNIFORM
    void OpenGLShader::SetUniformMatrix(const std::string &name, const glm::mat2 &matrices, const i32 count)
    {
        glUniformMatrix3fv(GetUniformLocation(name), count, false, glm::value_ptr(matrices));
    }

    void OpenGLShader::SetUniformMatrix(const std::string &name, const glm::mat3 &matrices, const i32 count)
    {
        glUniformMatrix3fv(GetUniformLocation(name), count, false, glm::value_ptr(matrices));
    }

    void OpenGLShader::SetUniformMatrix(const std::string &name, const glm::mat4 &matrices, const i32 count)
    {
        glUniformMatrix4fv(GetUniformLocation(name), count, false, glm::value_ptr(matrices));
    }

    i32 OpenGLShader::GetUniformLocation(const std::string &name)
    {
        if (m_UniformLocationCache.contains(name))
        {
            return m_UniformLocationCache[name];
        }

        const i32 location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1)
        {
            OGN_CORE_WARN("[Shader] Uniform '{}' does not exists or uninitialized", name);
            PUSH_CONSOLE_INFO("[Shader] Uniform '{}' does not exists or uninitialized", name);
        }

        m_UniformLocationCache[name] = location;
        return location;
    }
}