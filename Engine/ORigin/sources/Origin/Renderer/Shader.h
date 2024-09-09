// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SHADER_H
#define SHADER_H

#include "Origin/Core/Assert.h"

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <vector>

namespace origin
{
    namespace ShaderUtils
    {
        static const char *GetCacheDirectory()
        {
            return "Resources/Cache/Shaders/SPIRV";
        }

        static void CreateCachedDirectoryIfNeeded()
        {
            std::string cachedDirectory = GetCacheDirectory();
            if (!std::filesystem::exists(cachedDirectory))
            {
                std::filesystem::create_directories(cachedDirectory);
            }
        }
    }

    enum class ShaderType
    {
        NONE = -1,
        VERTEX,
        FRAGMENT,
        GEOMETRY
    };

    struct ShaderProgramSources
    {
        std::string VertexSources;
        std::string FragmentSources;
        std::string GeometrySources;
    };

    using ShaderData = std::unordered_map<uint32_t, std::vector<uint32_t>>;
    using ShaderSource = std::unordered_map<uint32_t, std::string>;

    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Enable() const {}
        virtual void Disable() const {}

        virtual void SetBool(const std::string &name, bool boolean) {}

        virtual void SetFloat(const std::string &name, f32 v0) {}
        virtual void SetFloat(const std::string &name, f32 v0, f32 v1) {}
        virtual void SetFloat(const std::string &name, f32 v0, f32 v1, f32 v2) {}
        virtual void SetFloat(const std::string &name, f32 v0, f32 v1, f32 v2, f32 v3) {}

        virtual void SetInt(const std::string &name, i32 v0) {}
        virtual void SetInt(const std::string &name, i32 v0, i32 v1) {}
        virtual void SetInt(const std::string &name, i32 v0, i32 v1, i32 v2) {}
        virtual void SetInt(const std::string &name, i32 v0, i32 v1, i32 v2, i32 v3) {}
        virtual void SetIntArray(const std::string &name, i32 *values, i32 count) {}

        virtual void SetVector(const std::string &name, const glm::vec2 &vectors, i32 count = 1) {}
        virtual void SetVector(const std::string &name, const glm::vec3 &vectors, i32 count = 1) {}
        virtual void SetVector(const std::string &name, const glm::vec4 &vectors, i32 count = 1) {}

        virtual void SetMatrix(const std::string &name, const glm::mat2 &matrices, i32 count = 1) {}
        virtual void SetMatrix(const std::string &name, const glm::mat3 &matrices, i32 count = 1) {}
        virtual void SetMatrix(const std::string &name, const glm::mat4 &matrices, i32 count = 1) {}

        virtual int GetUniformLocation(const std::string &name) { return 0; }

        // Pure Virtual
        [[nodiscard]] virtual const std::filesystem::path &GetFilepath() const = 0;
        [[nodiscard]] virtual bool IsSPIRV() const = 0;
        [[nodiscard]] virtual bool IsRecompile() const = 0;

        virtual void Reload() = 0;

        static std::shared_ptr<Shader> Create(const std::filesystem::path &filepath, bool isSpirv, bool recompile = false);
        static std::shared_ptr<Shader> Create(const std::filesystem::path &filepath, bool recompile = false);

        static ShaderProgramSources ParseShader(const std::string &filepath);
        static std::string ReadFile(const std::string &filepath);
        static ShaderSource PreProcess(const std::string &source, const std::string &filepath);

        static ShaderData CompileOrGetVulkanBinaries(const ShaderSource &shaderSources, const std::string &filepath);
        static ShaderData CompileOrGetOpenGLBinaries(ShaderSource &openglCode, const ShaderData &vulkanSpirv, const std::string &filepath);
        static void Reflect(uint32_t stage, const std::vector<uint32_t> &code);

        static u32 ShaderTypeFromString(const std::string &type, const std::string &filepath);
        static u32 GLShaderStageToShaderC(const u32 stage);
        static std::string GLShaderStageToString(const u32 stage);
        static const char *GLShaderStageCachedOpenGLFileExtension(const u32 stage);
        static const char *GLShaderStageCachedVulkanFileExtension(const u32 stage);
        static const char *ShaderDataTypeToString(const u32 type);
        static const char *ShaderDataTypeToString(const ShaderType type);
        static u32 GetVulkanShaderStage(const u32 stage);
    };
}

#endif