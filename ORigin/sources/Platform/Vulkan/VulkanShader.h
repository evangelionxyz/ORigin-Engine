// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include <vulkan.h>
#include <vector>
#include <filesystem>

namespace origin
{
    class VulkanShader
    {
    public:
        VulkanShader(const std::string &vertexPath, const std::string &fragmentPath);
        ~VulkanShader();
        
        VkShaderModule CreateModule(const std::string &filepath);
    };
}

#endif