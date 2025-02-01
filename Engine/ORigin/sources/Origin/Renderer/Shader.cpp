// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Shader.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.hpp"

#include "Origin/Core/ConsoleManager.h"

#include <glad/glad.h>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>
#include <shaderc/shaderc.hpp>

namespace origin {
u32 Shader::ShaderTypeFromString(const std::string &type, const std::string &filepath)
{
    if (type == "vertex")
        return GL_VERTEX_SHADER;
    if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;
    if (type == "geometry")
        return GL_GEOMETRY_SHADER;

    OGN_CORE_ASSERT(false, "[Shader] Unknown Shader Type '{}'", filepath);
    return 0;
}

u32 Shader::GLShaderStageToShaderC(const u32 stage)
{
    switch (stage)
    {
    case GL_VERTEX_SHADER:   return shaderc_glsl_vertex_shader;
    case GL_FRAGMENT_SHADER: return shaderc_glsl_fragment_shader;
    case GL_GEOMETRY_SHADER: return shaderc_glsl_geometry_shader;
    default:                 OGN_CORE_ASSERT(false, "[Shader] Invalid Shader Stage");
        return static_cast<shaderc_shader_kind>(0);
    }
}

std::string Shader::GLShaderStageToString(const u32 stage)
{
    switch (stage)
    {
    case GL_VERTEX_SHADER:
        return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER:
        return "GL_FRAGMENT_SHADER";
    case GL_GEOMETRY_SHADER:
        return "GL_GEOMETRY_SHADER";
    default:
        OGN_CORE_ASSERT(false, "[Shader] Invalid Shader Stage");
        return "";
    }
}

const char *Shader::GLShaderStageCachedOpenGLFileExtension(const u32 stage)
{
    switch (stage)
    {
    case GL_VERTEX_SHADER:   return ".cached_opengl.vert";
    case GL_FRAGMENT_SHADER: return ".cached_opengl.frag";
    case GL_GEOMETRY_SHADER: return ".cached_opengl.geom";
    default:                 OGN_CORE_ASSERT(false, "[Shader] Invalid Shader Stage");
        return nullptr;
    }
}

const char *Shader::GLShaderStageCachedVulkanFileExtension(const u32 stage)
{
    switch (stage)
    {
    case GL_VERTEX_SHADER:   return ".cached_vulkan.vert";
    case GL_FRAGMENT_SHADER: return ".cached_vulkan.frag";
    case GL_GEOMETRY_SHADER: return ".cached_vulkan.geom";
    default:                 OGN_CORE_ASSERT(false, "[Shader] Invalid Shader Stage");
        return nullptr;
    }
}

const char *Shader::ShaderDataTypeToString(const u32 type)
{
    switch (type)
    {
    case GL_VERTEX_SHADER:   return "Vertex";
    case GL_FRAGMENT_SHADER: return "Fragment";
    case GL_GEOMETRY_SHADER: return "Geometry";
    default:                 return nullptr;
    }
}

const char *Shader::ShaderDataTypeToString(const ShaderType type)
{
    switch (type)
    {
    case ShaderType::VERTEX:   return "VERTEX";
    case ShaderType::FRAGMENT: return "FRAGMENT";
    case ShaderType::GEOMETRY: return "GEOMETRY";
    default:                   return nullptr;
    }
}

u32 Shader::GetVulkanShaderStage(const u32 stage)
{
    switch (stage)
    {
    case GL_VERTEX_SHADER: return VK_SHADER_STAGE_VERTEX_BIT;
    case GL_FRAGMENT_SHADER: return VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    OGN_CORE_ASSERT(false, "[Vulkan Shader] Invalid stage.");
    return 0;
}

Ref<Shader> Shader::Create(const std::filesystem::path &filepath, bool recompile)
{
    switch (RendererAPI::GetAPI())
    {
    case RendererAPI::API::None:
        return nullptr;
    case RendererAPI::API::OpenGL:
        return CreateRef<OpenGLShader>(filepath, recompile);
    case RendererAPI::API::Vulkan:
        return CreateRef<VulkanShader>(filepath, recompile);
    default: OGN_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}

Ref<Shader> Shader::Create(const std::filesystem::path &filepath, bool isSpirv, bool recompile)
{
    switch (RendererAPI::GetAPI())
    {
    case RendererAPI::API::None:
        return nullptr;
    case RendererAPI::API::OpenGL:
        return CreateRef<OpenGLShader>(filepath, isSpirv, recompile);
    case RendererAPI::API::Vulkan:
        return CreateRef<VulkanShader>(filepath, recompile);
    default: OGN_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}

ShaderProgramSources Shader::ParseShader(const std::string &filepath)
{
    OGN_PROFILER_RENDERING();

    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss[3];
    auto type = ShaderType::NONE;

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
                type = ShaderType::GEOMETRY;
                ShaderDataTypeToString(type);
            }
        }
        else
        {
            // Fill the source code to stream
            ss[static_cast<int>(type)] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str(), ss[2].str() };
}

std::string Shader::ReadFile(const std::string &filepath)
{
    OGN_PROFILER_RENDERING();

    std::string result;
    if (std::ifstream in(filepath, std::ios::in | std::ios::binary); in)
    {
        in.seekg(0, std::ios::end);

        if (const auto size = in.tellg(); size != -1)
        {
            result.resize(size);
            in.seekg(0, std::ios::beg);
            in.read(&result[0], size);
        }
        else
        {
            PUSH_CONSOLE_ERROR("[Shader] Could not read from file '{}'", filepath);
            OGN_CORE_ASSERT(false, "[Shader] Could not read from file '{}'", filepath);
        }
    }
    else
    {
        PUSH_CONSOLE_ERROR("[Shader] Could not open file {}", filepath);
        OGN_CORE_ASSERT(false, "[Shader] Could not open file {}", filepath);
    }

    return result;
}

ShaderSource Shader::PreProcess(const std::string &source, const std::string &filepath)
{
    OGN_PROFILER_RENDERING();

    std::unordered_map<GLenum, std::string> shader_sources;

    const auto type_token = "// type";
    const size_t token_length = strlen(type_token);
    size_t pos = source.find(type_token, 0);

    while (pos != std::string::npos)
    {
        const size_t eol = source.find_first_of("\r\n", pos);
        if (eol == std::string::npos)
        {
            PUSH_CONSOLE_ERROR("[Shader] Syntax error");
            OGN_CORE_ASSERT(false, "[Shader] Syntax error");
        }

        const size_t begin = pos + token_length + 1;
        std::string type = source.substr(begin, eol - begin);

        if (!ShaderTypeFromString(type, filepath))
        {
            PUSH_CONSOLE_ERROR("[Shader] Invalid shader type specified");
            OGN_CORE_ASSERT(false, "[Shader] Invalid shader type specified");
        }

        const size_t next_line_pos = source.find_first_of("\r\n", eol);
        if (next_line_pos == std::string::npos)
        {
            PUSH_CONSOLE_ERROR("[Shader] Syntax Error");
            OGN_CORE_ASSERT(false, "[Shader] Syntax Error");
        }

        pos = source.find(type_token, next_line_pos);
        shader_sources[ShaderTypeFromString(type, filepath)] = (pos == std::string::npos)
            ? source.substr(next_line_pos)
            : source.substr(next_line_pos, pos - next_line_pos);
    }

    return shader_sources;
}

ShaderData Shader::CompileOrGetVulkanBinaries(const ShaderSource &shaderSources, const std::string &filepath)
{
    ShaderData shaderData;
    shaderc::CompileOptions options;
    std::filesystem::path cacheDirectory = ShaderUtils::GetCacheDirectory();

    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    for (auto &&[stage, source] : shaderSources)
    {
        std::filesystem::path shaderFilepath = filepath;
        std::filesystem::path cachedPath = cacheDirectory / (shaderFilepath.filename().string() + GLShaderStageCachedVulkanFileExtension(stage));
        if (std::ifstream infile(cachedPath, std::ios::in | std::ios::binary); infile.is_open())
        {
            infile.seekg(0, std::ios::end);
            auto size = infile.tellg();

            infile.seekg(0, std::ios::beg);

            auto &data = shaderData[stage];
            data.resize(size / sizeof(u32));

            infile.read(reinterpret_cast<char *>(data.data()), size);
        }
        else
        {
            shaderc::Compiler compiler;

            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, (shaderc_shader_kind)GLShaderStageToShaderC(stage), filepath.c_str());
            bool  success = module.GetCompilationStatus() == shaderc_compilation_status_success;

            PUSH_CONSOLE_ERROR("[Shader] Failed to compile Vulkan {}", module.GetErrorMessage().c_str());
            OGN_CORE_ASSERT(success, module.GetErrorMessage().c_str());

            shaderData[stage] = std::vector<u32>(module.cbegin(), module.cend());
            if (std::ofstream out(cachedPath, std::ios::out | std::ios::binary); out.is_open())
            {
                auto &data = shaderData[stage];
                out.write(reinterpret_cast<char *>(data.data()), data.size() * sizeof(u32));
                out.flush();
                out.close();
            }
        }
    }
    for (auto &&[stage, data] : shaderData)
    {
        Reflect(stage, data);
    }

    return shaderData;
}

ShaderData Shader::CompileOrGetOpenGLBinaries(ShaderSource &openglCode, const ShaderData &vulkanSpirv, const std::string &filepath)
{
    ShaderData              shaderData;
    shaderc::CompileOptions options;
    std::filesystem::path   cacheDirectory = ShaderUtils::GetCacheDirectory();

    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    openglCode.clear();
    for (auto &&[stage, spirv] : vulkanSpirv)
    {
        std::filesystem::path shaderFilepath = filepath;
        std::filesystem::path cachedPath = cacheDirectory / (shaderFilepath.filename().string() + GLShaderStageCachedOpenGLFileExtension(stage));

        if (std::ifstream infile(cachedPath, std::ios::in | std::ios::binary); infile.is_open())
        {
            infile.seekg(0, std::ios::end);
            auto size = infile.tellg();

            infile.seekg(0, std::ios::beg);

            auto &data = shaderData[stage];
            data.resize(size / sizeof(u32));
            infile.read(reinterpret_cast<char *>(data.data()), size);
        }
        else
        {
            shaderc::Compiler compiler;
            spirv_cross::CompilerGLSL glslCompiler(spirv);
            openglCode[stage] = glslCompiler.compile();
            auto &source = openglCode[stage];
            shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, (shaderc_shader_kind)GLShaderStageToShaderC(stage), filepath.c_str());
            bool success = module.GetCompilationStatus() == shaderc_compilation_status_success;
            shaderData[stage] = std::vector<u32>(module.cbegin(), module.cend());

            if (std::ofstream outfile(cachedPath, std::ios::out | std::ios::binary); outfile.is_open())
            {
                auto &data = shaderData[stage];
                outfile.write(reinterpret_cast<char *>(data.data()), data.size() * sizeof(u32));
                outfile.flush();
                outfile.close();
            }
        }
    }

    return shaderData;
}

void Shader::Reflect(const u32 stage, const std::vector<u32> &code)
{
    OGN_PROFILER_RENDERING();

    const spirv_cross::Compiler        compiler(code);
    auto [uniform_buffers, storage_buffers, stage_inputs,
        stage_outputs, subpass_inputs, storage_images,
        sampled_images, atomic_counters, acceleration_structures,
        gl_plain_uniforms, push_constant_buffers,
        shader_record_buffers, separate_images,
        separate_samplers, builtin_inputs,
        builtin_outputs] = compiler.get_shader_resources();

    OGN_CORE_TRACE("[Shader] Shader Reflect - {}", GLShaderStageToString(stage));
    OGN_CORE_TRACE("[Shader]     {} uniform buffers", uniform_buffers.size());
    OGN_CORE_TRACE("[Shader]     {} resources", sampled_images.size());

    if (!uniform_buffers.empty())
    {
        OGN_CORE_TRACE("[Shader] Uniform buffers:");
        for (const auto &[id, type_id, base_type_id, name] : uniform_buffers)
        {
            const auto &bufferType = compiler.get_type(base_type_id);
            u32 bufferSize = static_cast<u32>(compiler.get_declared_struct_size(bufferType));
            u32 binding = compiler.get_decoration(id, spv::DecorationBinding);
            size_t memberCount = bufferType.member_types.size();

            OGN_CORE_WARN("[Shader]      Name = {}", name);
            OGN_CORE_TRACE("[Shader]      Size = {}", bufferSize);
            OGN_CORE_TRACE("[Shader]   Binding = {}", binding);
            OGN_CORE_TRACE("[Shader]   Members = {}", memberCount);
        }
    }
}

}
