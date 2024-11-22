#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include <vulkan/vulkan_core.h>
#include <vector>

#include "ORigin/Core/Types.h"

namespace origin {

class VulkanSwapchain
{
public:
    using VkImages = std::vector<VkImage>;
    using VkImageViews = std::vector<VkImageView>;

    VulkanSwapchain() = default;
    VulkanSwapchain(VkDevice device, VkAllocationCallbacks *allocator, VkSurfaceKHR surface,
        VkSurfaceFormatKHR surfaceFormat, VkSurfaceCapabilitiesKHR capabilities, VkPresentModeKHR presentMode,
        VkImageUsageFlags imageUsageFlags, u32 queueFamilyIndex);

    void Destroy(VkDevice device, const VkAllocationCallbacks *allocator);
    const VkSwapchainKHR *GetVkSwapchain();
    VkImages GetVkImages() const;
    VkImage GetVkImage(u32 index);
    VkImageViews GetVkImageViews() const;
    const VkImageView &GetVkImageView(u32 index) const;
    VkSurfaceFormatKHR GetVkFormat() const;
    u32 GetVkImageCount() const;
    u32 GkVkMinImageCount() const;

private:
    void CreateImageViews(VkDevice device, VkAllocationCallbacks *allocator, u32 imageCount);

    VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
    VkSurfaceFormatKHR m_Format;
    VkImages m_Images;
    VkImageViews m_ImageViews;
    u32 m_MinImageCount = 0;
};
}

#endif