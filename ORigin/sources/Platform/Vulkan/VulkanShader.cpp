// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "VulkanShader.h"
#include "VulkanContext.h"

#include <fstream>
#include <glad/glad.h>

namespace origin
{
    static VkShaderStageFlagBits GetShaderStage(uint32_t stage)
    {
        switch(stage)
        {
        case GL_VERTEX_SHADER  : return VK_SHADER_STAGE_VERTEX_BIT;
        case GL_FRAGMENT_SHADER: return VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        OGN_CORE_ASSERT(false, "[Vulkan Shader] Invalid stage.");
        return (VkShaderStageFlagBits)0;
    }

    VulkanShader::VulkanShader(const std::filesystem::path &filepath, bool recompile)
        : m_Filepath(filepath), m_IsRecompile(recompile)
    {
        OGN_CORE_ASSERT(std::filesystem::exists(filepath), "[Vulkan Shader] Filepath does not exists {}", filepath);
        ShaderUtils::CreateCachedDirectoryIfNeeded();
        std::string source         = Shader::ReadFile(filepath.string());
        ShaderSource shaderSources = Shader::PreProcess(source, filepath.string());
        ShaderData vulkanSpirv     = Shader::CompileOrGetVulkanBinaries(shaderSources, filepath.string());
        VulkanContext *context     = VulkanContext::GetInstance();

        for (auto &[stage, spirv] : vulkanSpirv)
        {
            VkShaderModule shaderModule = CreateModule(spirv);
            VkPipelineShaderStageCreateInfo createInfo{};
            createInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            createInfo.stage  = GetShaderStage(stage);
            createInfo.module = shaderModule;
            createInfo.pName  = "main";
            m_Stages.push_back(createInfo);
            vkDestroyShaderModule(context->m_Device, shaderModule, nullptr);
        }
    }

    VulkanShader::~VulkanShader()
    {
    }

    VkShaderModule VulkanShader::CreateModule(const std::vector<uint32_t> &spirv)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = spirv.size() * sizeof(uint32_t);
        createInfo.pCode = spirv.data();
        VulkanContext *context = VulkanContext::GetInstance();
        VkShaderModule shaderModule;
        vkCreateShaderModule(context->m_Device, &createInfo, nullptr, &shaderModule);

        return shaderModule;
    }

    void VulkanShader::Reload()
    {
    }

    const std::filesystem::path &VulkanShader::GetFilepath() const
    {
        return m_Filepath;
    }
}