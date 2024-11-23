#include "pch.h"
#include "VulkanGraphicsPipeline.hpp"

#include "VulkanWrapper.hpp"

namespace origin {

VulkanGraphicsPipeline::VulkanGraphicsPipeline()
    : m_Device(VK_NULL_HANDLE), m_RenderPass(VK_NULL_HANDLE), m_Allocator(VK_NULL_HANDLE),
    m_PipelineLayout(VK_NULL_HANDLE), m_GraphicsPipeline(VK_NULL_HANDLE)
{
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice device, VkRenderPass render_pass, 
    VkAllocationCallbacks *allocator) : m_Device(device), m_RenderPass(render_pass), m_Allocator(allocator),
    m_PipelineLayout(VK_NULL_HANDLE), m_GraphicsPipeline(VK_NULL_HANDLE)
{
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
}

void VulkanGraphicsPipeline::Create(
    const std::vector<VkPipelineShaderStageCreateInfo> &shader_stages,
    const VkPipelineVertexInputStateCreateInfo &vertex_input_info,
    const VkPipelineInputAssemblyStateCreateInfo &input_assembly_info, 
    const VkViewport &viewport, 
    const VkRect2D &scissor, 
    const VkPipelineRasterizationStateCreateInfo &rasterization_info,
    const VkPipelineMultisampleStateCreateInfo &multisample_info,
    const VkPipelineColorBlendStateCreateInfo &color_blend_info, 
    const VkPipelineLayoutCreateInfo &layout_info)
{
    m_Viewport = viewport;
    m_Scissor = scissor;

    // create pipeline layout
    VkResult result = vkCreatePipelineLayout(m_Device, &layout_info, nullptr, &m_PipelineLayout);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create pipeline layout");

    // viewport state
    VkPipelineViewportStateCreateInfo viewportInfo = {};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;

    // dynamic states
    std::vector<VkDynamicState> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
    dynamicStateInfo.pDynamicStates = dynamic_states.data();

    // graphics pipeline creation info
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shader_stages.size());
    pipelineInfo.pStages = shader_stages.data();
    pipelineInfo.pVertexInputState = &vertex_input_info;
    pipelineInfo.pInputAssemblyState = &input_assembly_info;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterization_info;
    pipelineInfo.pMultisampleState = &multisample_info;
    pipelineInfo.pColorBlendState = &color_blend_info;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = m_RenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, 
        &pipelineInfo, m_Allocator, &m_GraphicsPipeline);

    VK_ERROR_CHECK(result, "[Vulkan] Failed to create graphics pipeline");
}

void VulkanGraphicsPipeline::Cleanup()
{
    if (m_GraphicsPipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(m_Device, m_GraphicsPipeline, m_Allocator);
    }

    if (m_PipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(m_Device, m_PipelineLayout, m_Allocator);
    }
}

void VulkanGraphicsPipeline::Resize(u32 width, u32 height)
{
    m_Viewport.width = width;
    m_Viewport.height = height;
    m_Scissor.extent.width = width;
    m_Scissor.extent.height = height;
}

void VulkanGraphicsPipeline::BeginRenderPass(VkCommandBuffer command_buffer, VkFramebuffer framebuffer, VkClearValue clear_value, u32 width, u32 height)
{
    const VkRect2D render_area = { { 0, 0 },{ width, height } };

    VkRenderPassBeginInfo render_pass_begin_info = {};
    render_pass_begin_info.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.pNext           = VK_NULL_HANDLE;
    render_pass_begin_info.renderPass      = m_RenderPass;
    render_pass_begin_info.renderArea      = render_area;
    render_pass_begin_info.framebuffer     = framebuffer;
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues    = &clear_value;

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanGraphicsPipeline::EndRenderPass(VkCommandBuffer command_buffer)
{
    vkCmdEndRenderPass(command_buffer);
}

}