#include "pch.h"
#include "VulkanQueue.hpp"
#include "VulkanWrapper.hpp"

namespace origin {

VulkanQueue::VulkanQueue(VkDevice device, VkSwapchainKHR swapchain, VkAllocationCallbacks *allocator, u32 queueFamilyIndex, u32 queueIndex)
    : m_Device(device), m_Swapchain(swapchain), m_Allocator(allocator)
{
    vkGetDeviceQueue(m_Device, queueFamilyIndex, queueIndex, &m_Queue);
    OGN_CORE_INFO("[Vulkan] Queue Acquired");
    CreateSemaphores();
}

u32 VulkanQueue::AcquiredNextImage() const
{
    u32 image_index = 0;
    VkResult result = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &image_index);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to acquired image at index {}", image_index);
    return image_index;
}

void VulkanQueue::SubmitSync(VkCommandBuffer cmd) const
{
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = VK_NULL_HANDLE;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = VK_NULL_HANDLE;
    submitInfo.pWaitDstStageMask = VK_NULL_HANDLE;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = VK_NULL_HANDLE;

    VK_ERROR_CHECK(vkQueueSubmit(m_Queue, 1, &submitInfo, m_InFlightFence), "[Vulkan] Failed to submit");
}

void VulkanQueue::SubmitAsync(VkCommandBuffer cmd) const
{
    // submit command buffer
    VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore };
    VkSemaphore signaledSemaphores[] = { m_RenderFinishedSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1u;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 1u;
    submitInfo.pSignalSemaphores = signaledSemaphores;
    submitInfo.waitSemaphoreCount = 1u;
    submitInfo.pWaitSemaphores = waitSemaphores;

    vkQueueSubmit(m_Queue, 1u, &submitInfo, m_InFlightFence);
}

void VulkanQueue::Present(u32 imageIndex) const
{
    VkSemaphore signaledSemaphores[] = { m_RenderFinishedSemaphore };
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1u;
    presentInfo.pWaitSemaphores = signaledSemaphores;
    presentInfo.swapchainCount = 1u;
    presentInfo.pSwapchains = &m_Swapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = VK_NULL_HANDLE;

    vkQueuePresentKHR(m_Queue, &presentInfo);
}

void VulkanQueue::WaitIdle() const
{
    vkQueueWaitIdle(m_Queue);
}

void VulkanQueue::Destroy() const
{
    vkDestroySemaphore(m_Device, m_ImageAvailableSemaphore, m_Allocator);
    vkDestroySemaphore(m_Device, m_RenderFinishedSemaphore, m_Allocator);
    vkDestroyFence(m_Device, m_InFlightFence, m_Allocator);
}

void VulkanQueue::WaitAndResetFences() const
{
    // wait for the previous frame to finish
    vkWaitForFences(m_Device, 1u, &m_InFlightFence, VK_TRUE, UINT64_MAX);
    // reset the fence to be signaled again later
    vkResetFences(m_Device, 1, &m_InFlightFence);
}

VkQueue VulkanQueue::GetVkQueue() const
{
    return m_Queue;
}

void VulkanQueue::CreateSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFence);
    vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore);
    vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore);
}

}
