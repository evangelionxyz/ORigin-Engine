// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Shader.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLShader.h"
#ifdef OGN_PLATFORM_LINUX
#   include "Platform/Vulkan/VulkanShader.h"
#endif
#include "Origin/Core/ConsoleManager.h"

#include <glad/glad.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <shaderc/shaderc.hpp>

namespace origin
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

    std::shared_ptr<Shader> Shader::Create(const std::filesystem::path &filepath, bool recompile)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(filepath, recompile);
#ifdef OGN_PLATFORM_LINUX
        case RendererAPI::API::VULKAN:
            return std::make_shared<VulkanShader>(filepath, recompile);
#endif
        }
        OGN_CORE_ASSERT(false, "Unkown RendererAPI");
        return nullptr;

    }

    std::shared_ptr<Shader> Shader::Create(const std::filesystem::path &filepath, bool isSpirv, bool recompile)
    {
        switch (RendererAPI::GetAPI())
        {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(filepath, isSpirv, recompile);
#ifdef OGN_PLATFORM_LINUX
        case RendererAPI::API::VULKAN:
            return nullptr; // std::make_shared<VulkanShader>(filepath, recompile);
#endif
        }

        OGN_CORE_ASSERT(false, "Unkown RendererAPI");
        return nullptr;
    }

    ShaderProgramSources Shader::ParseShader(const std::string &filepath)
    {
        OGN_PROFILER_RENDERING();

        std::ifstream stream(filepath);

        std::string line;
        std::stringstream ss[3];
        ShaderType type = ShaderType::NONE;

        while (getline(stream, line))
        {
            // Finding first line for types
            if (line.find("// type ") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                {
                    type = ShaderType::VERTEX;
                    ShaderDataTypeToString(type);
                }
                else if (line.find("fragment") != std::string::npos)
                {
                    type = ShaderType::FRAGMENT;
                    ShaderDataTypeToString(type);
                }
                else if (line.find("geometry") != std::string::npos)
                {
                    type = ShaderType::GEOMTERY;
                    ShaderDataTypeToString(type);
                }
            }
            else
            {
                // Fill the source code to stream
                ss[(int)type] << line << "\n";
            }
        }

        return {ss[0].str(), ss[1].str(), ss[2].str()};
    }

    std::string Shader::ReadFile(const std::string &filepath)
    {
        OGN_PROFILER_RENDERING();

        std::string result;
        std::ifstream in(filepath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            size_t size = in.tellg();
            if (size != -1)
            {
                result.resize(size);
                in.seekg(0, std::ios::beg);
                in.read(&result[0], size);
            }
            else
            {
                PUSH_CONSOLE_ERROR(    "[Shader] Could not read from file '{0}'", filepath);
                OGN_CORE_ASSERT(false, "[Shader] Could not read from file '{0}'", filepath);
            }
        }
        else
        {
            PUSH_CONSOLE_ERROR(    "[Shader] Could not open file {0}", filepath);
            OGN_CORE_ASSERT(false, "[Shader] Could not open file {0}", filepath);
        }

        return result;
    }

    ShaderSource Shader::PreProcess(const std::string &source, const std::string &filepath)
    {
        OGN_PROFILER_RENDERING();

        std::unordered_map<GLenum, std::string> shaderSources;

        const char *typeToken  = "// type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos             = source.find(typeToken, 0);

        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);
            if (eol == std::string::npos)
            {
                PUSH_CONSOLE_ERROR(    "[Shader] Syntax error");
                OGN_CORE_ASSERT(false, "[Shader] Syntax error");
            }

            size_t begin     = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            if (!ShaderTypeFromString(type, filepath))
            {
                PUSH_CONSOLE_ERROR(    "[Shader] Invalid shader type specified");
                OGN_CORE_ASSERT(false, "[Shader] Invalid shader type specified");
            }

            size_t nextLinePos = source.find_first_of("\r\n", eol);
            if (nextLinePos == std::string::npos)
            {
                PUSH_CONSOLE_ERROR(    "[Shader] Syntax Error");
                OGN_CORE_ASSERT(false, "[Shader] Syntax Error");
            }

            pos = source.find(typeToken, nextLinePos);
            shaderSources[ShaderTypeFromString(type, filepath)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }

    ShaderData Shader::CompileOrGetVulkanBinaries(const ShaderSource &shaderSources, const std::string &filepath)
    {
        ShaderData              shaderData;
        shaderc::Compiler       compiler;
        shaderc::CompileOptions options;
        std::filesystem::path   cacheDirectory = ShaderUtils::GetCacheDirectory();

        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        for (auto &&[stage, source] : shaderSources)
        {
            std::filesystem::path shaderFilepath = filepath;
            std::filesystem::path cachedPath     = cacheDirectory / (shaderFilepath.filename().string() + GLShaderStageCachedVulkanFileExtension(stage));
            std::ifstream         infile(cachedPath, std::ios::in | std::ios::binary);
            if (infile.is_open())
            {
                infile.seekg(0, std::ios::end);
                auto size = infile.tellg();
                infile.seekg(0, std::ios::beg);
                auto &data = shaderData[stage];
                data.resize(size / sizeof(uint32_t));
                infile.read((char *)data.data(), size);
            }
            else
            {
                shaderc::SpvCompilationResult module   = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), filepath.c_str());
                bool                          succsess = module.GetCompilationStatus() == shaderc_compilation_status_success;

                PUSH_CONSOLE_ERROR("[Shader] Failed to compile Vulkan {0}", module.GetErrorMessage().c_str());
                OGN_CORE_ASSERT(succsess, module.GetErrorMessage());

                shaderData[stage] = std::vector<uint32_t>(module.cbegin(), module.cend());
                std::ofstream out(cachedPath, std::ios::out | std::ios::binary);
                if (out.is_open())
                {
                    auto &data = shaderData[stage];
                    out.write((char *)data.data(), data.size() * sizeof(uint32_t));
                    out.flush();
                    out.close();
                }
            }
        }
        for (auto &&[stage, data] : shaderData)
        {
            Shader::Reflect(stage, data);
        }

        return shaderData;
    }

    ShaderData Shader::CompileOrGetOpenGLBinaries(ShaderSource &openglCode, const ShaderData &vulkanSpirv, const std::string &filepath)
    {
        ShaderData              shaderData;
        shaderc::Compiler       compiler;
        shaderc::CompileOptions options;
        std::filesystem::path   cacheDirectory = ShaderUtils::GetCacheDirectory();

        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        openglCode.clear();
        for (auto &&[stage, spirv] : vulkanSpirv)
        {
            std::filesystem::path shaderFilepath = filepath;
            std::filesystem::path cachedPath     = cacheDirectory / (shaderFilepath.filename().string() + GLShaderStageCachedOpenGLFileExtension(stage));

            std::ifstream infile(cachedPath, std::ios::in | std::ios::binary);
            if (infile.is_open())
            {
                infile.seekg(0, std::ios::end);
                auto size = infile.tellg();
                infile.seekg(0, std::ios::beg);
                auto &data = shaderData[stage];
                data.resize(size / sizeof(uint32_t));
                infile.read((char *)data.data(), size);
            }
            else
            {
                spirv_cross::CompilerGLSL glslCompiler(spirv);
                openglCode[stage]                    = glslCompiler.compile();
                auto &source                         = openglCode[stage];
                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, GLShaderStageToShaderC(stage), filepath.c_str());
                bool success                         = module.GetCompilationStatus() == shaderc_compilation_status_success;
                shaderData[stage]                    = std::vector<uint32_t>(module.cbegin(), module.cend());
                std::ofstream outfile(cachedPath, std::ios::out | std::ios::binary);
                if (outfile.is_open())
                {
                    auto &data = shaderData[stage];
                    outfile.write((char *)data.data(), data.size() * sizeof(uint32_t));
                    outfile.flush();
                    outfile.close();
                }
            }
        }

        return shaderData;
    }

    void Shader::Reflect(uint32_t stage, const std::vector<uint32_t> &code)
    {
        OGN_PROFILER_RENDERING();

        spirv_cross::Compiler        compiler(code);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();
        OGN_CORE_TRACE("[Shader] Shader Reflect - {0}", GLShaderStageToString(stage));
        OGN_CORE_TRACE("[Shader]     {0} uniform buffers", resources.uniform_buffers.size());
        OGN_CORE_TRACE("[Shader]     {0} resources", resources.sampled_images.size());

        if (resources.uniform_buffers.size())
        {
            OGN_CORE_TRACE("[Shader] Uniform buffers:");
            for (const auto &resource : resources.uniform_buffers)
            {
                const auto &bufferType = compiler.get_type(resource.base_type_id);
                uint32_t bufferSize    = compiler.get_declared_struct_size(bufferType);
                uint32_t binding       = compiler.get_decoration(resource.id, spv::DecorationBinding);
                int memberCount        = bufferType.member_types.size();

                OGN_CORE_WARN( "[Shader]      Name = {0}", resource.name);
                OGN_CORE_TRACE("[Shader]      Size = {0}", bufferSize);
                OGN_CORE_TRACE("[Shader]   Binding = {0}", binding);
                OGN_CORE_TRACE("[Shader]   Members = {0}", memberCount);
            }
        }
    }


}