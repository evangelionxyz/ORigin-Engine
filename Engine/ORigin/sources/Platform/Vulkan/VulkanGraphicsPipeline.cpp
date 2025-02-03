#include "pch.h"
#include "VulkanGraphicsPipeline.hpp"

#include "VulkanWrapper.hpp"

namespace origin {

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const PipelineCreateInfo &create_info)
    : m_create_info(create_info)
{
    // create pipeline layout
    VkResult result = vkCreatePipelineLayout(VulkanContext::GetInstance()->GetVkDevice(), &create_info.layout, nullptr, &m_pipeline_layout);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create pipeline layout");

    // viewport state
    VkPipelineViewportStateCreateInfo viewport_state = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewport_state.viewportCount = 1;
    viewport_state.pViewports = &create_info.viewport;
    viewport_state.scissorCount = 1;
    viewport_state.pScissors = &create_info.scissor;

    // dynamic states
    std::array<VkDynamicState, 2> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamic_state.dynamicStateCount = static_cast<u32>(dynamic_states.size());
    dynamic_state.pDynamicStates = dynamic_states.data();

    // graphics pipeline creation info
    VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipelineInfo.stageCount = static_cast<u32>(create_info.shader_stages.size());
    pipelineInfo.pStages = create_info.shader_stages.data();
    pipelineInfo.pVertexInputState = &create_info.vertex_input_state;
    pipelineInfo.pInputAssemblyState = &create_info.input_assembly_state;
    pipelineInfo.pViewportState = &viewport_state;
    pipelineInfo.pRasterizationState = &create_info.rasterization_state;
    pipelineInfo.pMultisampleState = &create_info.multisample_state;
    pipelineInfo.pColorBlendState = &create_info.color_blend_state;
    pipelineInfo.pDynamicState = &dynamic_state;
    pipelineInfo.layout = m_pipeline_layout;
    pipelineInfo.renderPass = create_info.render_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    result = vkCreateGraphicsPipelines(VulkanContext::GetInstance()->GetVkDevice(),
        VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline);

    VK_ERROR_CHECK(result, "[Vulkan] Failed to create graphics pipeline");
}

void VulkanGraphicsPipeline::Destroy()
{
    vkDestroyPipeline(VulkanContext::GetInstance()->GetVkDevice(), m_pipeline, nullptr);
    vkDestroyPipelineLayout(VulkanContext::GetInstance()->GetVkDevice(), m_pipeline_layout, nullptr);
}

void VulkanGraphicsPipeline::Bind(VkCommandBuffer cmd)
{
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
}

}