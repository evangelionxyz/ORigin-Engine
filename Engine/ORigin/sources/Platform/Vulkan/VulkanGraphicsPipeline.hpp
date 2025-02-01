// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef VULKAN_GRAPHICS_PIPLINE_HPP
#define VULKAN_GRAPHICS_PIPLINE_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

namespace origin {

class VulkanGraphicsPipeline
{
public:
    VulkanGraphicsPipeline();
    VulkanGraphicsPipeline(VkDevice device);
    ~VulkanGraphicsPipeline();
    
    void Create(
        const std::vector<VkPipelineShaderStageCreateInfo> &shaderStages,
        const VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
        const VkPipelineInputAssemblyStateCreateInfo &inputAssemblyInfo,
        const VkViewport &viewport,
        const VkRect2D &scissor,
        const VkRenderPass &render_pass,
        const VkPipelineRasterizationStateCreateInfo &rasterizationInfo,
        const VkPipelineMultisampleStateCreateInfo &multisampleInfo,
        const VkPipelineColorBlendStateCreateInfo &colorBlendInfo,
        const VkPipelineLayoutCreateInfo &layoutInfo
    );

    void Destroy();
    VkPipeline GetPipeline() const { return m_pipeline; }
    VkPipelineLayout GetPipelineLayout() const { return m_pipeline_layout; }
private:
    VkDevice m_device;
    VkPipelineLayout m_pipeline_layout;
    VkPipeline m_pipeline;
};
}

#endif

