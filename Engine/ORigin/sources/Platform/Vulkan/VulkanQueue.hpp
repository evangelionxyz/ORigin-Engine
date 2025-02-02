#ifndef VULKAN_QUEUE_H
#define VULKAN_QUEUE_H

#include <vulkan/vulkan.h>
#include "Origin/Core/Types.h"

namespace origin {

class VulkanQueue
{
public:
    VulkanQueue() = default;
    explicit VulkanQueue(VkDevice device, u32 queueFamilyIndex, u32 queueIndex);

    void SubmitSync(VkCommandBuffer cmd) const;
    void SubmitAsync(VkCommandBuffer cmd) const;
    VkResult Present(u32 imageIndex, VkSwapchainKHR swapchain) const;
    void WaitIdle() const;
    void Destroy() const;
    void WaitAndResetFences() const;

    VkSemaphore GetSemaphore() const;

    [[nodiscard]] VkQueue GetVkQueue() const;
private:

    void CreateSemaphores();

    VkDevice m_Device = VK_NULL_HANDLE;
    VkQueue m_Queue = VK_NULL_HANDLE;
    VkSemaphore m_ImageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_RenderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence m_InFlightFence = VK_NULL_HANDLE;
    mutable u32 m_ImageIndex = 0;
};
}

#endif