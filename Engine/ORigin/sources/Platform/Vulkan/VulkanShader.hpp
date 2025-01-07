// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "Origin/Renderer/Shader.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <filesystem>

namespace origin {
class VulkanShader final : public Shader
{
public:
    VulkanShader(const std::filesystem::path &filepath, bool recompile);
    ~VulkanShader() override;

    void Reload()      override;
    bool IsSPIRV()     const override { return true; }
    bool IsRecompile() const override { return m_IsRecompile; }
    const std::filesystem::path &GetFilepath() const override;
    std::vector< VkPipelineShaderStageCreateInfo> GetShaderStages() { return m_Stages; }

private:
    VkShaderModule CreateModule(const std::vector<u32> &spirv);
    bool m_IsRecompile = false;
    std::filesystem::path m_Filepath;
    std::vector<VkShaderModule> m_Modules;
    std::vector<VkPipelineShaderStageCreateInfo> m_Stages;
};
}

#endif