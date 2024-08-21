// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Renderer.h"
#include "Shader.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Origin/Core/ConsoleManager.h"

#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace origin
{
    std::shared_ptr<Shader> Shader::Create(const std::filesystem::path &filepath, bool recompile)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(filepath, recompile);
        case RendererAPI::API::VULKAN:
            return std::make_shared<VulkanShader>(filepath, recompile);
        }
        OGN_CORE_ASSERT(false, "Unkown RendererAPI");
        return nullptr;

    }

    std::shared_ptr<Shader> Shader::Create(const std::filesystem::path &filepath, bool isSpirv, bool recompile)
    {
        switch (Renderer::GetAPI())
        {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return std::make_shared<OpenGLShader>(filepath, isSpirv, recompile);
        case RendererAPI::API::VULKAN:
            return nullptr; // std::make_shared<VulkanShader>(filepath, recompile);
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
            if (line.find("//type ") != std::string::npos || line.find("// type ") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos || line.find("Vertex") != std::string::npos)
                {
                    type = ShaderType::VERTEX;
                    ShaderUtils::ShaderDataTypeToString(type);
                }
                else if (line.find("fragment") != std::string::npos || line.find("Fragment") != std::string::npos)
                {
                    type = ShaderType::FRAGMENT;
                    ShaderUtils::ShaderDataTypeToString(type);
                }
                else if (line.find("geometry") != std::string::npos || line.find("Geometry") != std::string::npos)
                {
                    type = ShaderType::GEOMTERY;
                    ShaderUtils::ShaderDataTypeToString(type);
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
                PUSH_CONSOLE_ERROR("[Shader] Could not read from file '{0}'", filepath);
                OGN_CORE_ASSERT(false, "[Shader] Could not read from file '{0}'", filepath);
            }
        }
        else
        {
            PUSH_CONSOLE_ERROR("[Shader] Could not open file {0}", filepath);
            OGN_CORE_ASSERT(false, "[Shader] Could not open file {0}", filepath);
        }

        return result;
    }

    ShaderSource Shader::PreProcess(const std::string &source, const std::string &filepath)
    {
        OGN_PROFILER_RENDERING();

        std::unordered_map<GLenum, std::string> shaderSources;

        const char *typeToken = "// type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);

        while (pos != std::string::npos)
        {
            size_t eol = source.find_first_of("\r\n", pos);

            if (eol == std::string::npos)
            {
                PUSH_CONSOLE_ERROR("[Shader] Syntax error");
                OGN_CORE_ASSERT(false, "[Shader] Syntax error");
            }

            size_t begin = pos + typeTokenLength + 1;
            std::string type = source.substr(begin, eol - begin);
            if (!ShaderUtils::ShaderTypeFromString(type, filepath))
            {
                PUSH_CONSOLE_ERROR("[Shader] Invalid shader type specified");
                OGN_CORE_ASSERT(false, "[Shader] Invalid shader type specified");
            }

            size_t nextLinePos = source.find_first_of("\r\n", eol);
            if (nextLinePos == std::string::npos)
            {
                PUSH_CONSOLE_ERROR("[Shader] Syntax Error");
                OGN_CORE_ASSERT(false, "[Shader] Syntax Error");
            }

            pos = source.find(typeToken, nextLinePos);
            shaderSources[ShaderUtils::ShaderTypeFromString(type, filepath)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
        }

        return shaderSources;
    }

    ShaderData Shader::CompileOrGetVulkanBinaries(const ShaderSource &shaderSources, const std::string &filepath)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
        std::filesystem::path cacheDirectory = ShaderUtils::GetCacheDirectory();
        ShaderData shaderData;
        for (auto &&[stage, source] : shaderSources)
        {
            std::filesystem::path shaderFilepath = filepath;
            std::filesystem::path cachedPath = cacheDirectory / (shaderFilepath.filename().string() + ShaderUtils::GLShaderStageCachedVulkanFileExtension(stage));
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
                shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(source, ShaderUtils::GLShaderStageToShaderC(stage), filepath.c_str());
                bool succsess = module.GetCompilationStatus() == shaderc_compilation_status_success;
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
            Shader::Reflect(stage, data);
        return shaderData;
    }

    void Shader::Reflect(GLenum stage, const std::vector<uint32_t> &code)
    {
        OGN_PROFILER_RENDERING();

        spirv_cross::Compiler compiler(code);
        spirv_cross::ShaderResources resources = compiler.get_shader_resources();
        OGN_CORE_TRACE("[Shader] Shader Reflect - {0}", ShaderUtils::GLShaderStageToString(stage));
        OGN_CORE_TRACE("[Shader]     {0} uniform buffers", resources.uniform_buffers.size());
        OGN_CORE_TRACE("[Shader]     {0} resources", resources.sampled_images.size());

        if (resources.uniform_buffers.size())
        {
            OGN_CORE_TRACE("[Shader] Uniform buffers:");
            for (const auto &resource : resources.uniform_buffers)
            {
                const auto &bufferType = compiler.get_type(resource.base_type_id);
                uint32_t bufferSize = compiler.get_declared_struct_size(bufferType);
                uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
                int memberCount = bufferType.member_types.size();
                OGN_CORE_WARN("[Shader]      Name = {0}", resource.name);
                OGN_CORE_TRACE("[Shader]      Size = {0}", bufferSize);
                OGN_CORE_TRACE("[Shader]   Binding = {0}", binding);
                OGN_CORE_TRACE("[Shader]   Members = {0}", memberCount);
            }
        }
    }
}