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
    VulkanGraphicsPipeline(VkDevice device, VkRenderPass render_pass, VkAllocationCallbacks *allocator);
    ~VulkanGraphicsPipeline();
    
    void Create(
        const std::vector<VkPipelineShaderStageCreateInfo> &shaderStages,
        const VkPipelineVertexInputStateCreateInfo &vertexInputInfo,
        const VkPipelineInputAssemblyStateCreateInfo &inputAssemblyInfo,
        const VkViewport &viewport,
        const VkRect2D &scissor,
        const VkPipelineRasterizationStateCreateInfo &rasterizationInfo,
        const VkPipelineMultisampleStateCreateInfo &multisampleInfo,
        const VkPipelineColorBlendStateCreateInfo &colorBlendInfo,
        const VkPipelineLayoutCreateInfo &layoutInfo
    );

    void Cleanup();

    void Resize(u32 width, u32 height);

    void BeginRenderPass(VkCommandBuffer command_buffer, VkFramebuffer framebuffer, VkClearValue clear_value, u32 width, u32 height);
    void EndRenderPass(VkCommandBuffer command_buffer);

    VkPipeline GetPipeline() const { return m_GraphicsPipeline; }
    VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
    VkRect2D GetScissor() const { return m_Scissor; }
    VkViewport GetViewport() const { return m_Viewport; }

private:
    VkDevice m_Device;
    VkRenderPass m_RenderPass;
    VkPipelineLayout m_PipelineLayout;
    VkPipeline m_GraphicsPipeline;
    VkAllocationCallbacks *m_Allocator;

    VkViewport m_Viewport;
    VkRect2D m_Scissor;
};



}

#endif

