// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <vector>

#include <glad/glad.h>
#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace origin
{
    enum class ShaderType
    {
        NONE = -1,
        VERTEX,
        FRAGMENT,
        GEOMTERY
    };

    namespace ShaderUtils
    {
        static GLenum ShaderTypeFromString(const std::string &type, const std::string &filepath = std::string())
        {
            if (type == "vertex")
                return GL_VERTEX_SHADER;
            if (type == "fragment" || type == "pixel")
                return GL_FRAGMENT_SHADER;
            if (type == "geometry")
                return GL_GEOMETRY_SHADER;
            OGN_CORE_ASSERT(false, "[Shader] Unkown Shader Type '{}'", filepath);
            return 0;
        }

        static shaderc_shader_kind GLShaderStageToShaderC(GLenum stage)
        {
            switch (stage)
            {
            case GL_VERTEX_SHADER:
                return shaderc_glsl_vertex_shader;
            case GL_FRAGMENT_SHADER:
                return shaderc_glsl_fragment_shader;
            case GL_GEOMETRY_SHADER:
                return shaderc_glsl_geometry_shader;
            }

            OGN_CORE_ASSERT(false, "[Shader] Invalid Shader Stage");
            return (shaderc_shader_kind)0;
        }

        static std::string GLShaderStageToString(GLenum stage)
        {
            switch (stage)
            {
            case GL_VERTEX_SHADER:
                return "GL_VERTEX_SHADER";
            case GL_FRAGMENT_SHADER:
                return "GL_FRAGMENT_SHADER";
            case GL_GEOMETRY_SHADER:
                return "GL_GEOMETRY_SHADER";
            }

            OGN_CORE_ASSERT(false, "[Shader] Invalid Shader Stage");
            return nullptr;
        }

        static const char *GetCacheDirectory()
        {
            return "Resources/Cache/Shaders/SPIRV";
        }

        static void CreateCachedDirectoryIfNeeded()
        {
            std::string cachedDirectory = GetCacheDirectory();
            if (!std::filesystem::exists(cachedDirectory))
                std::filesystem::create_directories(cachedDirectory);
        }

        static const char *GLShaderStageCachedOpenGLFileExtension(uint32_t stage)
        {
            switch (stage)
            {
            case GL_VERTEX_SHADER:
                return ".cached_opengl.vert";
            case GL_FRAGMENT_SHADER:
                return ".cached_opengl.frag";
            case GL_GEOMETRY_SHADER:
                return ".cached_opengl.geom";
            }

            OGN_CORE_ASSERT(false, "[Shader] Invalid Shader Stage");
            return nullptr;
        }

        static const char *GLShaderStageCachedVulkanFileExtension(uint32_t stage)
        {
            switch (stage)
            {
            case GL_VERTEX_SHADER:
                return ".cached_vulkan.vert";
            case GL_FRAGMENT_SHADER:
                return ".cached_vulkan.frag";
            case GL_GEOMETRY_SHADER:
                return ".cached_vulkan.geom";
            }

            OGN_CORE_ASSERT(false, "[Shader] Invalid Shader Stage");
            return nullptr;
        }

        static const char *ShaderDataTypeToString(GLenum type)
        {
            switch (type)
            {
            case GL_VERTEX_SHADER:
                return "Vertex";
            case GL_FRAGMENT_SHADER:
                return "Fragment";
            case GL_GEOMETRY_SHADER:
                return "Geometry";
            }
            return nullptr;
        }

        static const char *ShaderDataTypeToString(ShaderType type)
        {
            switch (type)
            {
            case ShaderType::VERTEX:
                return "VERTEX";
            case ShaderType::FRAGMENT:
                return "FRAGMENT";
            case ShaderType::GEOMTERY:
                return "GEOMETRY";
            }
            return nullptr;
        }
    }

    struct ShaderProgramSources
    {
        std::string VertexSources;
        std::string FragmentSources;
        std::string GeometrySources;
    };

    using ShaderData = std::unordered_map<GLenum, std::vector<uint32_t>>;
    using ShaderSource = std::unordered_map<GLenum, std::string>;

    class Shader
    {
    public:
        virtual ~Shader() {}
        virtual void Enable() const {}
        virtual void Disable() const {}
        virtual void SetBool(const std::string &name, bool boolean) {}
        virtual void SetFloat(const std::string &name, float v0) {}
        virtual void SetFloat(const std::string &name, float v0, float v1) {}
        virtual void SetFloat(const std::string &name, float v0, float v1, float v2) {}
        virtual void SetFloat(const std::string &name, float v0, float v1, float v2, float v3) {}
        virtual void SetInt(const std::string &name, int v0) {}
        virtual void SetInt(const std::string &name, int v0, int v1) {}
        virtual void SetInt(const std::string &name, int v0, int v1, int v2) {}
        virtual void SetInt(const std::string &name, int v0, int v1, int v2, int v3) {}
        virtual void SetIntArray(const std::string &name, int *values, uint32_t count) {}
        virtual void SetVector(const std::string &name, const glm::vec2 &vector2, int count = 1) {}
        virtual void SetVector(const std::string &name, const glm::vec3 &vector3, int count = 1) {}
        virtual void SetVector(const std::string &name, const glm::vec4 &vector4, int count = 1) {}
        virtual void SetMatrix(const std::string &name, const glm::mat2 &matrices, int count = 1) {}
        virtual void SetMatrix(const std::string &name, const glm::mat3 &matrices, int count = 1) {}
        virtual void SetMatrix(const std::string &name, const glm::mat4 &matrices, int count = 1) {}
        virtual int GetUniformLocation(const std::string &name) { return 0; }

        // Pure Virtual
        virtual const std::filesystem::path &GetFilepath() const = 0;
        virtual bool IsSPIRV() const = 0;
        virtual bool IsRecompile() const = 0;
        virtual void Reload() = 0;

        static std::shared_ptr<Shader> Create(const std::filesystem::path &filepath, bool isSpirv, bool recompile = false);
        static std::shared_ptr<Shader> Create(const std::filesystem::path &filepath, bool recompile = false);

        static ShaderProgramSources ParseShader(const std::string &filepath);
        static std::string ReadFile(const std::string &filepath);
        static ShaderSource PreProcess(const std::string &source, const std::string &filepath);
        static ShaderData CompileOrGetVulkanBinaries(const ShaderSource &shaderSources, const std::string &filepath);
        static void Reflect(GLenum stage, const std::vector<uint32_t> &code);
    };
}

#endif