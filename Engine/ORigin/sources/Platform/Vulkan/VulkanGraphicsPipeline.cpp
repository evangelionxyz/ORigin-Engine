#include "pch.h"
#include "VulkanGraphicsPipeline.hpp"

#include "VulkanWrapper.hpp"

namespace origin {

VulkanGraphicsPipeline::VulkanGraphicsPipeline()
    : m_device(VK_NULL_HANDLE), m_pipeline_layout(VK_NULL_HANDLE), m_pipeline(VK_NULL_HANDLE)
{
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice device)
    : m_device(device), m_pipeline_layout(VK_NULL_HANDLE), m_pipeline(VK_NULL_HANDLE)
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
    const VkRenderPass &render_pass,
    const VkPipelineRasterizationStateCreateInfo &rasterization_info,
    const VkPipelineMultisampleStateCreateInfo &multisample_info,
    const VkPipelineColorBlendStateCreateInfo &color_blend_info, 
    const VkPipelineLayoutCreateInfo &layout_info)
{
    // create pipeline layout
    VkResult result = vkCreatePipelineLayout(m_device, &layout_info, nullptr, &m_pipeline_layout);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create pipeline layout");

    // viewport state
    VkPipelineViewportStateCreateInfo viewportInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;

    // dynamic states
    std::array<VkDynamicState, 2> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicStateInfo.dynamicStateCount = static_cast<u32>(dynamic_states.size());
    dynamicStateInfo.pDynamicStates = dynamic_states.data();

    // graphics pipeline creation info
    VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipelineInfo.stageCount = static_cast<u32>(shader_stages.size());
    pipelineInfo.pStages = shader_stages.data();
    pipelineInfo.pVertexInputState = &vertex_input_info;
    pipelineInfo.pInputAssemblyState = &input_assembly_info;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterization_info;
    pipelineInfo.pMultisampleState = &multisample_info;
    pipelineInfo.pColorBlendState = &color_blend_info;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.layout = m_pipeline_layout;
    pipelineInfo.renderPass = render_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create graphics pipeline");
}

void VulkanGraphicsPipeline::Destroy()
{
    if (m_pipeline != VK_NULL_HANDLE)
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
    if (m_pipeline_layout != VK_NULL_HANDLE)
        vkDestroyPipelineLayout(m_device, m_pipeline_layout, nullptr);
}
}