#include "pch.h"
#include "VulkanSwapchain.hpp"
#include "VulkanWrapper.hpp"

namespace origin {

VulkanSwapchain::VulkanSwapchain(VkDevice device,
    VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat,
    VkSurfaceCapabilitiesKHR capabilities, VkPresentModeKHR presentMode,
    VkImageUsageFlags imageUsageFlags, u32 queueFamilyIndex)
    : m_Format(surfaceFormat), m_Device(device)
{
    m_MinImageCount = VkChooseImagesCount(capabilities);

    m_ImageExtent = capabilities.currentExtent;

    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.minImageCount = m_MinImageCount;
    create_info.imageFormat = surfaceFormat.format;
    create_info.imageColorSpace = surfaceFormat.colorSpace;
    create_info.imageExtent = m_ImageExtent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = imageUsageFlags;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices = &queueFamilyIndex;
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = presentMode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(m_Device, &create_info, nullptr, &m_Swapchain);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create swapchain");
    OGN_CORE_INFO("[Vulkan] Swapchain created");

    // create swapchain images
    u32 swapchainImageCount = 0;
    result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &swapchainImageCount, nullptr);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to get swapchain count");
    OGN_CORE_ASSERT(m_MinImageCount <= swapchainImageCount, "[Vulkan] Swapchain image count exceeds maximum number of images");
    OGN_CORE_INFO("[Vulkan] Requested {} images, created {} images ", m_MinImageCount, swapchainImageCount);

    CreateImageViews(m_Device, swapchainImageCount);
}

void VulkanSwapchain::Destroy()
{
    for (const auto imageView : m_ImageViews)
        vkDestroyImageView(m_Device, imageView, nullptr);

    OGN_CORE_INFO("[Vulkan] Swapchain image view destroyed");
    vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
    OGN_CORE_INFO("[Vulkan] Swapchain destroyed");
}

const VkSwapchainKHR VulkanSwapchain::GetVkSwapchain()
{
    return m_Swapchain;
}

VulkanSwapchain::VkImages VulkanSwapchain::GetVkImages() const
{
    return m_Images;
}

VkImage VulkanSwapchain::GetVkImage(u32 index)
{
    return m_Images[index];
}

origin::VulkanSwapchain::VkImageViews VulkanSwapchain::GetVkImageViews() const
{
    return m_ImageViews;
}

VkExtent2D VulkanSwapchain::GetVkExtent2D() const
{
    return m_ImageExtent;
}

const VkImageView &VulkanSwapchain::GetVkImageView(u32 index) const
{
    return m_ImageViews[index];
}

VkSurfaceFormatKHR VulkanSwapchain::GetVkFormat() const
{
    return m_Format;
}

u32 VulkanSwapchain::GetVkImageCount() const
{
    return static_cast<u32>(m_Images.size());
}

u32 VulkanSwapchain::GkVkMinImageCount() const
{
    return m_MinImageCount;
}

void VulkanSwapchain::CreateImageViews(VkDevice device, u32 image_count)
{
    m_Images.resize(image_count);
    m_ImageViews.resize(image_count);

    const VkResult result = vkGetSwapchainImagesKHR(device, m_Swapchain, 
        &image_count, m_Images.data());

    VK_ERROR_CHECK(result, "[Vulkan] Failed to get swapchain images");

    for (u32 i = 0; i < image_count; ++i)
    {
        constexpr i32 mip_levels = 1;
        constexpr i32 layer_count = 1;
        m_ImageViews[i] = VkCreateImageView(device, m_Images[i], nullptr,
            m_Format.format, VK_IMAGE_ASPECT_COLOR_BIT, 
            VK_IMAGE_VIEW_TYPE_2D, layer_count, mip_levels);
    }
}

}