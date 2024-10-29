#include "pch.h"
#include "VulkanSwapchain.h"
#include "VulkanWrapper.h"

namespace origin {

VulkanSwapchain::VulkanSwapchain(VkDevice device, VkAllocationCallbacks *allocator,
    VkSurfaceKHR surface, VkSurfaceFormatKHR surfaceFormat,
    VkSurfaceCapabilitiesKHR capabilities, VkPresentModeKHR presentMode,
    VkImageUsageFlags imageUsageFlags, u32 queueFamilyIndex)
    : m_Format(surfaceFormat)
{
    m_MinImageCount = VkChooseImagesCount(capabilities);

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = m_MinImageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = capabilities.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = imageUsageFlags;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 1;
    swapchainCreateInfo.pQueueFamilyIndices = &queueFamilyIndex;
    swapchainCreateInfo.preTransform = capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, allocator, &m_Swapchain);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create swapchain");
    OGN_CORE_INFO("[Vulkan] Swapchain created");


    // create swapchain images
    u32 swapchainImageCount = 0;
    result = vkGetSwapchainImagesKHR(device, m_Swapchain, &swapchainImageCount, nullptr);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to get swapchain count");
    OGN_CORE_ASSERT(m_MinImageCount <= swapchainImageCount, "[Vulkan] Swapchain image count exceeds maximum number of images");
    OGN_CORE_INFO("[Vulkan] Requested {} images, created {} images ", m_MinImageCount, swapchainImageCount);

    CreateImageViews(device, allocator, swapchainImageCount);
}

void VulkanSwapchain::Destroy(VkDevice device, const VkAllocationCallbacks *allocator)
{
    for (const auto imageView : m_ImageViews)
        vkDestroyImageView(device, imageView, allocator);
    OGN_CORE_INFO("[Vulkan] Swapchain image view destroyed");

    vkDestroySwapchainKHR(device, m_Swapchain, allocator);
    OGN_CORE_INFO("[Vulkan] Swapchain destroyed");
}

const VkSwapchainKHR *VulkanSwapchain::GetVkSwapchain()
{
    return &m_Swapchain;
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

void VulkanSwapchain::CreateImageViews(VkDevice device, VkAllocationCallbacks *allocator, u32 imageCount)
{
    m_Images.resize(imageCount);
    m_ImageViews.resize(imageCount);

    const VkResult result = vkGetSwapchainImagesKHR(device, m_Swapchain, &imageCount, m_Images.data());
    VK_ERROR_CHECK(result, "[Vulkan] Failed to get swapchain images");

    for (u32 i = 0; i < imageCount; ++i)
    {
        constexpr i32 mipLevels = 1;
        constexpr i32 layerCount = 1;
        m_ImageViews[i] = VkCreateImageView(device, m_Images[i], allocator,
            m_Format.format, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, layerCount, mipLevels);
    }
}

}