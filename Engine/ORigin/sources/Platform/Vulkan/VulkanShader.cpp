// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanShader.hpp"
#include "VulkanContext.hpp"
#include "VulkanWrapper.hpp"

namespace origin {
VulkanShader::VulkanShader(const std::filesystem::path &filepath, bool recompile)
    : m_Filepath(filepath), m_IsRecompile(recompile)
{
    OGN_CORE_ASSERT(std::filesystem::exists(filepath), "[Vulkan Shader] Filepath does not exists {}",
        filepath.generic_string());

    ShaderUtils::CreateCachedDirectoryIfNeeded();
    const std::string source = ReadFile(filepath.string());
    const ShaderSource shaderSources = PreProcess(source, filepath.string());
    ShaderData vulkanSpirv = CompileOrGetVulkanBinaries(shaderSources, filepath.string(), recompile);

    const VulkanContext *context = VulkanContext::GetInstance();

    for (auto &[stage, spirv] : vulkanSpirv)
    {
        VkShaderModule shader_module = CreateModule(spirv);

        VkPipelineShaderStageCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        create_info.stage = (VkShaderStageFlagBits)Shader::GetVulkanShaderStage(stage);
        create_info.module = shader_module;
        create_info.pName = "main";

        m_Stages.push_back(create_info);
        m_Modules.push_back(shader_module);
    }
}

VulkanShader::~VulkanShader()
{
    const VulkanContext *context = VulkanContext::GetInstance();
    for (auto &module : m_Modules)
        vkDestroyShaderModule(context->m_LogicalDevice, module, nullptr);
}

VkShaderModule VulkanShader::CreateModule(const std::vector<u32> &spirv)
{
    VkShaderModule module;
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = spirv.size() * sizeof(u32);
    create_info.pCode = spirv.data();

    const VulkanContext *vk_context = VulkanContext::GetInstance();

    VkResult result = vkCreateShaderModule(vk_context->m_LogicalDevice, &create_info, nullptr, &module);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create shader module");

    return module;
}

void VulkanShader::Reload()
{
}

const std::filesystem::path &VulkanShader::GetFilepath() const
{
    return m_Filepath;
}
}