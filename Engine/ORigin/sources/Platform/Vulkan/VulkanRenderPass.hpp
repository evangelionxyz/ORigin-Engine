// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "vulkan/vulkan.h"
#include <span>

namespace origin {

class VulkanRenderPass
{
public:
    VulkanRenderPass() = default;
    VulkanRenderPass(const std::vector<VkAttachmentDescription> &attachments, 
        const std::vector<VkSubpassDescription> &subpasses, 
        const std::vector<VkSubpassDependency> &dependencies);

    void Begin(VkCommandBuffer cmd, const VkRect2D &render_area, VkFramebuffer framebuffer, VkClearValue clear_value);
    void End(VkCommandBuffer cmd);

    void Destroy();

    VkRenderPass GetRenderPass() const { return m_render_pass; }

private:
    VkRenderPass  m_render_pass = VK_NULL_HANDLE;
};

}