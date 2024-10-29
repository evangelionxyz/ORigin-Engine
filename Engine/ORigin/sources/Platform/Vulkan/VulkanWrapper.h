#ifndef VULKAN_WRAPPER_H
#define VULKAN_WRAPPER_H

#include "ORigin/Core/Assert.h"
#include "ORigin/Core/Log.h"

#define VK_ERROR_CHECK(result, ...)\
{\
    if (result != VK_SUCCESS){\
        OGN_CORE_ERROR("[Vulkan] Assertion failed at {}: line {}", __FILE__, __LINE__);\
        OGN_CORE_ERROR(__VA_ARGS__);\
        OGN_DEBUGBREAK();\
    }\
}

namespace origin
{

static VkBool32 VkDebugMessengerCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT  messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT         messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void*                                       pUserData
)
{
    OGN_CORE_INFO("[Vulkan Message]: ");
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        OGN_CORE_ERROR("\tVulkan: {0}", pCallbackData->pMessage);
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        OGN_CORE_WARN("\tVulkan: {0}", pCallbackData->pMessage);
    }

    if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        OGN_CORE_INFO("\tGeneral: {0}", pCallbackData->pMessageIdName);
    }
    if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        OGN_CORE_INFO("\tValidation: {0}", pCallbackData->pMessageIdName);
    }
    if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        OGN_CORE_INFO("\tPerformance: {0}", pCallbackData->pMessageIdName);
    }

    // Return VK_FALSE to indicate that the application should not be terminated
    // If VK_TRUE is returned, the application will be terminated after the callback returns
    return VK_FALSE;
}

static VkPresentModeKHR VkChoosePresentMode(const std::vector<VkPresentModeKHR>& presentMode)
{
    for (const auto mode : presentMode)
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) return mode;
    return presentMode[0];
}

static u32 VkChooseImagesCount(const VkSurfaceCapabilitiesKHR& capabilities)
{
    const u32 requested_image_count = capabilities.minImageCount + 1;
    u32 final_image_count = 0;
    if (capabilities.maxImageCount > 0 && requested_image_count > capabilities.maxImageCount)
        final_image_count = capabilities.maxImageCount;
    else
        final_image_count = requested_image_count;
    return final_image_count;
}

static VkSurfaceFormatKHR VkChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
    for (const auto format : formats)
        if (format.format == VK_FORMAT_B8G8R8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    return formats[0];
}

static void VkBeginCommandBuffer(VkCommandBuffer cmd, VkCommandBufferUsageFlags usageFlags)
{
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = usageFlags;
    begin_info.pNext = VK_NULL_HANDLE;
    begin_info.pInheritanceInfo = VK_NULL_HANDLE;

    VkResult result = vkBeginCommandBuffer(cmd, &begin_info);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to begin command buffer");
}

static VkSemaphore VkCreateSemaphore(VkDevice device, VkAllocationCallbacks* allocator)
{
    VkSemaphoreCreateInfo semaphore_info = {};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.flags = 0;
    semaphore_info.pNext = VK_NULL_HANDLE;

    VkSemaphore semaphore = VK_NULL_HANDLE;
    VK_ERROR_CHECK(vkCreateSemaphore(device, &semaphore_info, allocator, &semaphore), "[Vulkan] Failed to create semaphore");
    OGN_CORE_ASSERT(semaphore != VK_NULL_HANDLE, "[Vulkan] Semaphore is null");
    return semaphore;
}

static VkImageView VkCreateImageView(const VkDevice device, const VkImage image, VkAllocationCallbacks* allocator,
    VkFormat format, VkImageAspectFlags aspectFlags, VkImageViewType viewType, u32 layerCount, u32 mipLevels)
{
    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.flags = 0;
    view_info.image = image;
    view_info.viewType = viewType;
    view_info.format = format;
    view_info.components = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };
    view_info.subresourceRange = {
        .aspectMask = aspectFlags,
        .baseMipLevel = 0,
        .levelCount = mipLevels,
        .baseArrayLayer = 0,
        .layerCount = layerCount,
    };

    VkImageView view = VK_NULL_HANDLE;
    const VkResult result = vkCreateImageView(device, &view_info, allocator, &view);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create image view");
    return view;
}

static void VkPrintImageUsageFlags(const VkImageUsageFlags usage)
{
    if (usage & VK_IMAGE_USAGE_SAMPLED_BIT)
    {
        OGN_CORE_INFO("\t[Vulkan] Sampled is supported");
    }
    else if (usage & VK_IMAGE_USAGE_STORAGE_BIT)
    {
        OGN_CORE_INFO("\t[Vulkan] Storage is supported");
    }
    else if (usage & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
    {
        OGN_CORE_INFO("\t[Vulkan] Input attachment is supported");
    }
    else if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        OGN_CORE_INFO("\t[Vulkan] Depth stencil attachment is supported");
    }
    else if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
    {
        OGN_CORE_INFO("\t[Vulkan] Color attachment is supported");
    }
    else if (usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    {
        OGN_CORE_INFO("\t[Vulkan] Transfer dst is supported");
    }
    else if (usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
    {
        OGN_CORE_INFO("\t[Vulkan] Transfer src is supported");
    }
}

static void VkPrintMemoryProperty(VkMemoryPropertyFlags properties)
{
    if (properties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    {
        OGN_CORE_INFO("DEVICE LOCAL ");
    }
    else if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        OGN_CORE_INFO("HOST VISIBLE ");
    }
    else if (properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    {
        OGN_CORE_INFO("HOST COHERENT ");
    }
    else if (properties & VK_MEMORY_PROPERTY_HOST_CACHED_BIT)
    {
        OGN_CORE_INFO("HOST CACHED ");
    }
    else if (properties & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)
    {
        OGN_CORE_INFO("LAZILY ALLOCATED ");
    }
    else if (properties & VK_MEMORY_PROPERTY_PROTECTED_BIT)
    {
        OGN_CORE_INFO("PROTECTED ");
    }
}

}

#endif
