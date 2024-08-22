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
    OpenGLShader::OpenGLShader(const std::filesystem::path &filepath, bool recompile)
        : m_Filepath(filepath), m_RendererID(0), m_IsSPIRV(true), m_IsRecompile(recompile)
    {
        PUSH_CONSOLE_INFO("[Shader] Trying to load Shader {}", m_Filepath);
        OGN_CORE_TRACE("[Shader] Trying to load Shader {}", m_Filepath);

        ShaderUtils::CreateCachedDirectoryIfNeeded();
        std::string source = Shader::ReadFile(filepath.string());
        {
            auto shaderSources = Shader::PreProcess(source, filepath.string());
            Timer timer;
            ShaderData vulkanSpirv = CompileOrGetVulkanBinaries(shaderSources, filepath.string());
            m_OpenGLSPIRV = Shader::CompileOrGetOpenGLBinaries(m_OpenGLSourceCode, vulkanSpirv, filepath.string());
            CreateSpirvProgram();
            PUSH_CONSOLE_INFO("[Shader] Shader Creation took {0} ms", timer.ElapsedMillis());
            OGN_CORE_TRACE("[Shader] Shader Creation took {0} ms", timer.ElapsedMillis());
        }
    }

    OpenGLShader::OpenGLShader(const std::filesystem::path &filepath, bool spirv, bool recompile)
        : m_Filepath(filepath), m_RendererID(0), m_IsSPIRV(spirv), m_IsRecompile(recompile)
    {
        OGN_PROFILER_RENDERING();
        PUSH_CONSOLE_INFO("[Shader] Trying to load Shader {}", m_Filepath);
        OGN_CORE_TRACE("[Shader] Trying to load Shader {}", m_Filepath);

        // With SPIRV
        if (spirv)
        {
            Timer timer;
            ShaderUtils::CreateCachedDirectoryIfNeeded();
            std::string source = Shader::ReadFile(filepath.string());
            ShaderSource shaderSources = Shader::PreProcess(source, filepath.string());
            ShaderData vulkanSpirv = CompileOrGetVulkanBinaries(shaderSources, filepath.string());
            m_OpenGLSPIRV = CompileOrGetOpenGLBinaries(m_OpenGLSourceCode, vulkanSpirv, filepath.string());
            CreateSpirvProgram();
            PUSH_CONSOLE_INFO("[Shader] Shader Creation took {0} ms", timer.ElapsedMillis());
            OGN_CORE_TRACE("[Shader] Shader Creation took {0} ms", timer.ElapsedMillis());
        }
        else // Without SPIRV
        {
            m_ShaderSource = Shader::ParseShader(filepath.string());
            m_RendererID = CreateProgram(m_ShaderSource.VertexSources, m_ShaderSource.FragmentSources, m_ShaderSource.GeometrySources);
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

        uint32_t program = glCreateProgram();
        std::vector<uint32_t> shaderIDs;
        for (auto &&[stage, spirv] : m_OpenGLSPIRV)
        {
            uint32_t shaderID = shaderIDs.emplace_back(glCreateShader(stage));
            glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, spirv.data(), spirv.size() * sizeof(uint32_t));
            glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);
            glAttachShader(program, shaderID);
        }

        glLinkProgram(program);
        glValidateProgram(program);

        int isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
        if (isLinked < 0)
        {
            OGN_CORE_ERROR("[Shader] Failed to linking shader");
            PUSH_CONSOLE_ERROR("[Shader] Failed to linking shader");
            return;
        }

        if (isLinked == GL_FALSE)
        {
            int maxLength;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
            std::vector<GLchar> infolog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, infolog.data());
            glDeleteProgram(program);
            for (auto id : shaderIDs)
            {
                glDeleteShader(id);
            }

            return;
        }

        for (auto id : shaderIDs)
            glDeleteShader(id);

        m_RendererID = program;
    }

    uint32_t OpenGLShader::CompileShader(uint32_t type, const std::string &source)
    {
        OGN_PROFILER_RENDERING();

        uint32_t shaderID = glCreateShader(type);
        const char *src = source.c_str();
        glShaderSource(shaderID, 1, &src, nullptr);
        glCompileShader(shaderID);

        int success;
        char infoLog[512];
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
            const char *m = "[Shader] Failed to Compile ";
            const char *shaderType{};

            switch (type)
            {
            case GL_VERTEX_SHADER:
                shaderType = "VERTEX TYPE";
                break;
            case GL_FRAGMENT_SHADER:
                shaderType = "FRAGMENT TYPE";
                break;
            case GL_GEOMETRY_SHADER:
                shaderType = "GEOMETRY TYPE";
                break;
            }
            size_t len = strlen(m) + strlen(shaderType) + 1;
            char *msg = (char *)malloc(len);

            if (msg != NULL)
            {
                strcpy(msg, m);
                strcat(msg, shaderType);
                OGN_CORE_ERROR(msg);
                PUSH_CONSOLE_ERROR(msg);
                free(msg);
            }
            PUSH_CONSOLE_ERROR("[Shader] {0}", infoLog);
            OGN_CORE_ASSERT(false, "[Shader] {0}", infoLog);
            return 0;
        }

        switch (type)
        {
        case GL_VERTEX_SHADER:
            OGN_CORE_TRACE("[Shader] VERTEX Succesfully Compiled");
            PUSH_CONSOLE_INFO("[Shader] VERTEX Succesfully Compiled");
            break;
        case GL_FRAGMENT_SHADER:
            OGN_CORE_TRACE("[Shader] FRAGMENT Succesfully Compiled");
            PUSH_CONSOLE_INFO("[Shader] FRAGMENT Succesfully Compiled");
            break;
        case GL_GEOMETRY_SHADER:
            OGN_CORE_TRACE("[Shader] GEOMETRY Succesfully Compiled");
            PUSH_CONSOLE_INFO("[Shader] GEOMETRY Succesfully Compiled");
            break;
        }

        return shaderID;
    }

    uint32_t OpenGLShader::CreateProgram(std::string vertexSrc, std::string fragmentSrc, std::string geometrySrc)
    {
        OGN_PROFILER_RENDERING();

        // Create Program
        uint32_t shaderProgram = glCreateProgram();
        uint32_t vShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
        uint32_t fShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);
        if (!geometrySrc.empty())
        {
            uint32_t gShader = CompileShader(GL_GEOMETRY_SHADER, geometrySrc);
            glAttachShader(shaderProgram, gShader);
        }

        // Attach and Link Shader->Program
        glAttachShader(shaderProgram, vShader);
        glAttachShader(shaderProgram, fShader);
        glLinkProgram(shaderProgram);

        glValidateProgram(shaderProgram);

        glDeleteShader(vShader);
        glDeleteShader(fShader);

        return shaderProgram;
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
            std::string source = Shader::ReadFile(m_Filepath.string());
            {
                auto shaderSources = Shader::PreProcess(source, m_Filepath.string());

                Timer timer;
                ShaderData vulkanSpirv = CompileOrGetVulkanBinaries(shaderSources, m_Filepath.string());
                m_OpenGLSPIRV = CompileOrGetOpenGLBinaries(m_OpenGLSourceCode, vulkanSpirv, m_Filepath.string());
                CreateSpirvProgram();
                OGN_CORE_TRACE("[Shader] Shader Creation took {0} ms", timer.ElapsedMillis());
                PUSH_CONSOLE_INFO("[Shader] Shader Creation took {0} ms", timer.ElapsedMillis());
            }
            m_IsRecompile = false;
        }
        else
        {
            m_ShaderSource = Shader::ParseShader(m_Filepath.string());
            m_RendererID = CreateProgram(m_ShaderSource.VertexSources, m_ShaderSource.FragmentSources, m_ShaderSource.GeometrySources);
        }
    }

    void OpenGLShader::SetBool(const std::string &name, bool boolean)
    {
        SetUniformBool(name, boolean);
    }

    void OpenGLShader::SetFloat(const std::string &name, float v0)
    {
        SetUniformFloat(name, v0);
    }

    void OpenGLShader::SetFloat(const std::string &name, float v0, float v1)
    {
        SetUniformFloat(name, v0, v1);
    }

    void OpenGLShader::SetFloat(const std::string &name, float v0, float v1, float v2)
    {
        SetUniformFloat(name, v0, v1, v2);
    }

    void OpenGLShader::SetFloat(const std::string &name, float v0, float v1, float v2, float v3)
    {
        SetUniformFloat(name, v0, v1, v2, v3);
    }

    void OpenGLShader::SetInt(const std::string &name, int v0)
    {
        SetUniformInt(name, v0);
    }

    void OpenGLShader::SetInt(const std::string &name, int v0, int v1)
    {
        SetUniformInt(name, v0, v1);
    }

    void OpenGLShader::SetInt(const std::string &name, int v0, int v1, int v2)
    {
        SetUniformInt(name, v0, v1, v2);
    }

    void OpenGLShader::SetInt(const std::string &name, int v0, int v1, int v2, int v3)
    {
        SetUniformInt(name, v0, v1, v2, v3);
    }

    void OpenGLShader::SetIntArray(const std::string &name, int *values, uint32_t count)
    {
        SetUniformIntArray(name, values, count);
    }

    void OpenGLShader::SetVector(const std::string &name, const glm::vec2 &vectors, int count)
    {
        SetUniformVector(name, vectors, count);
    }

    void OpenGLShader::SetVector(const std::string &name, const glm::vec3 &vectors, int count)
    {
        SetUniformVector(name, vectors, count);
    }

    void OpenGLShader::SetVector(const std::string &name, const glm::vec4 &vectors, int count)
    {
        SetUniformVector(name, vectors, count);
    }

    void OpenGLShader::SetMatrix(const std::string &name, const glm::mat2 &matrices, int count)
    {
        SetUniformMatrix(name, matrices, count);
    }

    void OpenGLShader::SetMatrix(const std::string &name, const glm::mat3 &matrices, int count)
    {
        SetUniformMatrix(name, matrices, count);
    }

    void OpenGLShader::SetMatrix(const std::string &name, const glm::mat4 &matrices, int count)
    {
        SetUniformMatrix(name, matrices, count);
    }

    // private
    // BOOLEAN UNIFORM
    void OpenGLShader::SetUniformBool(const std::string &name, bool boolean)
    {
        glUniform1i(GetUniformLocation(name), boolean);
    }

    // FLOAT UNIFORM
    void OpenGLShader::SetUniformFloat(const std::string &name, float v0)
    {
        glUniform1f(GetUniformLocation(name), v0);
    }
    void OpenGLShader::SetUniformFloat(const std::string &name, float v0, float v1)
    {
        glUniform2f(GetUniformLocation(name), v0, v1);
    }
    void OpenGLShader::SetUniformFloat(const std::string &name, float v0, float v1, float v2)
    {
        glUniform3f(GetUniformLocation(name), v0, v1, v2);
    }
    void OpenGLShader::SetUniformFloat(const std::string &name, float v0, float v1, float v2, float v3)
    {
        glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
    }
    // INT UNIFORM
    void OpenGLShader::SetUniformInt(const std::string &name, int v0)
    {
        glUniform1i(GetUniformLocation(name), v0);
    }

    void OpenGLShader::SetUniformInt(const std::string &name, int v0, int v1)
    {
        glUniform2i(GetUniformLocation(name), v0, v1);
    }

    void OpenGLShader::SetUniformInt(const std::string &name, int v0, int v1, int v2)
    {
        glUniform3i(GetUniformLocation(name), v0, v1, v2);
    }

    void OpenGLShader::SetUniformInt(const std::string &name, int v0, int v1, int v2, int v3)
    {
        glUniform4i(GetUniformLocation(name), v0, v1, v2, v3);
    }

    void OpenGLShader::SetUniformIntArray(const std::string &name, int *values, uint32_t count)
    {
        glUniform1iv(GetUniformLocation(name), count, values);
    }

    // VECTOR UNIFORM
    void OpenGLShader::SetUniformVector(const std::string &name, const glm::vec2 &vector2, int count)
    {
        glUniform2fv(GetUniformLocation(name), count, glm::value_ptr(vector2));
    }

    void OpenGLShader::SetUniformVector(const std::string &name, const glm::vec3 &vector3, int count)
    {
        glUniform3fv(GetUniformLocation(name), count, glm::value_ptr(vector3));
    }

    void OpenGLShader::SetUniformVector(const std::string &name, const glm::vec4 &vector4, int count)
    {
        glUniform4fv(GetUniformLocation(name), count, glm::value_ptr(vector4));
    }

    // MATRIX UNIFORM
    void OpenGLShader::SetUniformMatrix(const std::string &name, const glm::mat2 &matrices, int count)
    {
        glUniformMatrix3fv(GetUniformLocation(name), count, false, glm::value_ptr(matrices));
    }

    void OpenGLShader::SetUniformMatrix(const std::string &name, const glm::mat3 &matrices, int count)
    {
        glUniformMatrix3fv(GetUniformLocation(name), count, false, glm::value_ptr(matrices));
    }

    void OpenGLShader::SetUniformMatrix(const std::string &name, const glm::mat4 &matrices, int count)
    {
        glUniformMatrix4fv(GetUniformLocation(name), count, false, glm::value_ptr(matrices));
    }

    int OpenGLShader::GetUniformLocation(const std::string &name)
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        int location = glGetUniformLocation(m_RendererID, name.c_str());
        if (location == -1)
        {
            OGN_CORE_WARN("[Shader] WARNING UNIFORM '{}' does not exists or uninitialized", name);
            PUSH_CONSOLE_INFO("[Shader] WARNING UNIFORM '{}' does not exists or uninitialized", name);
        }

        m_UniformLocationCache[name] = location;
        return location;
    }
}