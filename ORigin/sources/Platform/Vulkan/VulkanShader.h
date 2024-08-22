// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "Origin/Renderer/Shader.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <filesystem>

namespace origin
{
    class VulkanShader : public Shader
    {
    public:
        VulkanShader(const std::filesystem::path &filepath, bool recompile);
        ~VulkanShader();

        void                         Reload()      override;
        bool                         IsSPIRV()     const override { return true; }
        bool                         IsRecompile() const override { return m_IsRecompile; }
        const std::filesystem::path &GetFilepath() const override;

    private:
        VkShaderModule CreateModule(const std::vector<uint32_t> &spirv);
        bool                                         m_IsRecompile = false;
        std::filesystem::path                        m_Filepath;
        std::vector<VkPipelineShaderStageCreateInfo> m_Stages;
    };
}

#endif