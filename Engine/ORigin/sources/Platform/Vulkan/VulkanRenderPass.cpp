// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanContext.hpp"
#include "VulkanRenderPass.hpp"

#include "VulkanWrapper.hpp"

namespace origin {

VulkanRenderPass::VulkanRenderPass(const std::vector<VkAttachmentDescription> &attachments,
    const std::vector<VkSubpassDescription> &subpasses,
    const std::vector<VkSubpassDependency> &dependencies)
{
    VkRenderPassCreateInfo render_pass_ci = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    render_pass_ci.pAttachments = attachments.data();
    render_pass_ci.attachmentCount = static_cast<u32>(attachments.size());
    render_pass_ci.pSubpasses = subpasses.data();
    render_pass_ci.subpassCount = static_cast<u32>(subpasses.size());
    render_pass_ci.pDependencies = dependencies.data();
    render_pass_ci.dependencyCount = static_cast<u32>(dependencies.size());

    VkResult result = vkCreateRenderPass(VulkanContext::GetInstance()->GetVkDevice(), &render_pass_ci, nullptr, &m_render_pass);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create render pass");
}

void VulkanRenderPass::Begin(VkCommandBuffer cmd, const VkRect2D &render_area, VkFramebuffer framebuffer)
{
    VkRenderPassBeginInfo render_pass_begin_info = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    render_pass_begin_info.pNext = VK_NULL_HANDLE;
    render_pass_begin_info.renderPass = m_render_pass;
    render_pass_begin_info.renderArea = render_area;
    render_pass_begin_info.framebuffer = framebuffer;
    render_pass_begin_info.clearValueCount = 1;
    render_pass_begin_info.pClearValues = &m_clear_value;

    vkCmdBeginRenderPass(cmd, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanRenderPass::End(VkCommandBuffer cmd)
{
    vkCmdEndRenderPass(cmd);
}

void VulkanRenderPass::Destroy()
{
    vkDestroyRenderPass(VulkanContext::GetInstance()->GetVkDevice(), m_render_pass, nullptr);
}

}