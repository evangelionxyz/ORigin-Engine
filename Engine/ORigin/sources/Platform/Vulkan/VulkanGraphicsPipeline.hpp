// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef VULKAN_GRAPHICS_PIPLINE_HPP
#define VULKAN_GRAPHICS_PIPLINE_HPP

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

namespace origin {

struct PipelineCreateInfo
{
    VkRenderPass render_pass;
    VkPipelineVertexInputStateCreateInfo vertex_input_state;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
    std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineRasterizationStateCreateInfo rasterization_state;
    VkPipelineMultisampleStateCreateInfo multisample_state;
    VkPipelineColorBlendStateCreateInfo color_blend_state;
    VkPipelineLayoutCreateInfo layout;
};

class VulkanGraphicsPipeline
{
public:
    VulkanGraphicsPipeline() = default;
    VulkanGraphicsPipeline(const PipelineCreateInfo &create_info);
    
    void Destroy();
    VkPipeline GetPipeline() const { return m_pipeline; }
    VkPipelineLayout GetLayout() const { return m_pipeline_layout; }
private:
    VkPipeline m_pipeline;
    VkPipelineLayout m_pipeline_layout;
    PipelineCreateInfo m_create_info;
};
}

#endif

