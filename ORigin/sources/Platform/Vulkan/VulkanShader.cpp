// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "VulkanShader.h"

#include <fstream>
#include "VulkanContext.h"

namespace origin
{
    VulkanShader::VulkanShader(const std::string &vertexPath, const std::string &fragmentPath)
    {
        VkShaderModule vertexShader = CreateModule(vertexPath);
        VkShaderModule fragmentShader = CreateModule(fragmentPath);

        VkPipelineShaderStageCreateInfo vertexStageInfo{};
        vertexStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertexStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertexStageInfo.module = vertexShader;
        vertexStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragmentStageInfo{};
        fragmentStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragmentStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragmentStageInfo.module = fragmentShader;
        fragmentStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStage[] = { vertexStageInfo, fragmentStageInfo };

        VulkanContext *context = VulkanContext::GetInstance();
        vkDestroyShaderModule(context->m_Device,vertexShader, nullptr);
        vkDestroyShaderModule(context->m_Device, fragmentShader, nullptr);
    }

    VulkanShader::~VulkanShader()
    {
        //VulkanContext *context = VulkanContext::GetInstance();
        //vkDestroyShaderModule(context->m_Device, m_VertexShader, nullptr);
        //vkDestroyShaderModule(context->m_Device, m_FragmentShader, nullptr);
    }

    VkShaderModule VulkanShader::CreateModule(const std::string &filepath)
    {
        OGN_CORE_ASSERT(std::filesystem::exists(filepath), "[Vulkan Shader] Filepath does not exists {}", filepath);

        // std::ios::ate -> start reading at the end (ate)
        // std::ios::binary -> read file as binary file (avoid text transformations)
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);
        size_t fileSize = (size_t)file.tellg();
        std::vector<char> code(fileSize);

        // seek back to beggining of the file and read all the bytes at once
        file.seekg(0);
        file.read(code.data(), fileSize);
        file.close();

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VulkanContext *context = VulkanContext::GetInstance();
        VkShaderModule shaderModule;
        vkCreateShaderModule(context->m_Device, &createInfo, nullptr, &shaderModule);

        return shaderModule;
    }
}