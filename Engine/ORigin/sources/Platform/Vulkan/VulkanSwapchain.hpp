#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

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

    VulkanSwapchain(VkDevice device, VkSurfaceKHR surface,
        VkSurfaceFormatKHR surfaceFormat, VkSurfaceCapabilitiesKHR capabilities,
        VkPresentModeKHR presentMode, VkImageUsageFlags imageUsageFlags, 
        u32 queueFamilyIndex
    );

    void Destroy();
    const VkSwapchainKHR GetVkSwapchain();
    VkImages GetVkImages() const;
    VkImage GetVkImage(u32 index);
    VkImageViews GetVkImageViews() const;
    VkExtent2D GetVkExtent2D() const;
    const VkImageView &GetVkImageView(u32 index) const;
    VkSurfaceFormatKHR GetVkFormat() const;
    u32 GetVkImageCount() const;
    u32 GkVkMinImageCount() const;

private:
    void CreateImageViews(VkDevice device, u32 imageCount);

    VkSwapchainKHR m_Swapchain;
    VkSurfaceFormatKHR m_Format;
    VkImages m_Images;
    VkImageViews m_ImageViews;
    VkDevice m_Device;
    u32 m_MinImageCount = 0;
    VkExtent2D m_ImageExtent;
};
}

#endif