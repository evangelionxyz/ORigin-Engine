// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanShader.h"
#include "VulkanContext.h"

namespace origin
{
    VulkanShader::VulkanShader(const std::filesystem::path &filepath, const bool recompile)
        : m_Filepath(filepath), m_IsRecompile(recompile)
    {
        OGN_CORE_ASSERT(std::filesystem::exists(filepath), "[Vulkan Shader] Filepath does not exists {}",
            filepath.generic_string());

        ShaderUtils::CreateCachedDirectoryIfNeeded();
        const std::string source         = ReadFile(filepath.string());
        const ShaderSource shaderSources = PreProcess(source, filepath.string());
        ShaderData vulkanSpirv           = CompileOrGetVulkanBinaries(shaderSources, filepath.string());
        const VulkanContext *context     = VulkanContext::GetInstance();

        for (auto &[stage, spirv] : vulkanSpirv)
        {
            VkShaderModule shader_module = CreateModule(spirv);

            VkPipelineShaderStageCreateInfo create_info{};
            create_info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            create_info.stage  = (VkShaderStageFlagBits)Shader::GetVulkanShaderStage(stage);
            create_info.module = shader_module;
            create_info.pName  = "main";
            m_Stages.push_back(create_info);
            vkDestroyShaderModule(context->m_Device, shader_module, nullptr);
        }
    }

    VulkanShader::~VulkanShader()
    {
    }

    VkShaderModule VulkanShader::CreateModule(const std::vector<u32> &spirv)
    {
        VkShaderModule module;
        VkShaderModuleCreateInfo create_info{};
        create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = spirv.size() * sizeof(u32);
        create_info.pCode    = spirv.data();

        const VulkanContext *vk_context = VulkanContext::GetInstance();
        
        vkCreateShaderModule(vk_context->m_Device, &create_info, vk_context->m_Allocator, &module);
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