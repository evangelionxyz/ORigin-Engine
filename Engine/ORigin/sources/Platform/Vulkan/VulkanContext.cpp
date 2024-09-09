// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanContext.h"

#include <optional>

#ifdef OGN_PLATFORM_WINDOWS
#   define VK_USE_PLATFORM_WIN32_KHR
#   include <GLFW/glfw3native.h>
#endif

#include "Origin/Core/Window.h"

namespace origin
{
    // Swapchain helpers
    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities{};
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    struct QueueFamilyIndices
    {
        std::optional<u32> GraphicsFamily;
        std::optional<u32> PresentFamily;

        [[nodiscard]] bool IsComplete() const
        {
            return GraphicsFamily.has_value() && PresentFamily.has_value();
        }
    };

    static VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
    {
        for (const auto format : availableFormats)
        {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }
        return availableFormats[0];
    }

    static VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR> &presentModes)
    {
        for (const auto &mode : presentModes)
        {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return mode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const i32 width, const i32 height)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
        {
            return capabilities.currentExtent;
        }

        VkExtent2D actualExtent = { static_cast<u32>(width), static_cast<u32>(height) };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices;

        u32 family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, queueFamilies.data());

        i32 i = 0;
        for (const auto &queue_family : queueFamilies)
        {
            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.GraphicsFamily = i;

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
                indices.PresentFamily = i;
            if (indices.IsComplete())
                break;
            i++;
        }

        return indices;
    }

    static SwapchainSupportDetails QuerySwapchainCapabilities(const VkPhysicalDevice device, const VkSurfaceKHR surface)
    {
        SwapchainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);
        
        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount)
        {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.Formats.data());
        }

        return details;
    }

    // Vulkan context
    VulkanContext *VulkanContext::s_Instance = nullptr;
    VulkanContext::VulkanContext()
    {
        s_Instance = this;
    }

    VulkanContext *VulkanContext::GetInstance()
    {
        return s_Instance;
    }

    void VulkanContext::Shutdown()
    {
#ifdef OGN_VULKAN_API
        vkDestroyDescriptorPool(m_Device, m_DescriptorPool, m_Allocator);

        for (auto imageView : m_Swapchain.Views)
        {
            vkDestroyImageView(m_Device, imageView, m_Allocator);
        }

        vkDestroySwapchainKHR(m_Device, m_Swapchain.Handle, m_Allocator);
        vkDestroySurfaceKHR(m_Instance, m_Surface, m_Allocator);
        vkDestroyDevice(m_Device, m_Allocator);
        vkDestroyInstance(m_Instance, m_Allocator);
#endif
    }

    void VulkanContext::Init(Window *window)
    {
#ifdef OGN_VULKAN_API
        m_WindowHandle = static_cast<GLFWwindow*>(window->GetNativeWindow());

        m_ClearValue.color.float32[0] = 0.1f;
        m_ClearValue.color.float32[1] = 0.1f;
        m_ClearValue.color.float32[2] = 0.1f;
        m_ClearValue.color.float32[3] = 1.0f;

        CreateVkInstance();
        CreateVkPhysicalDevice();
        CreateVkDevice();
        CreateVkDescriptorPool();

        CreateVkSurface();

        // CreateVkSwapchain();
        // CreateVkImageViews();

        {
            u32 instance_api_version = 0;
            vkEnumerateInstanceVersion(&instance_api_version);
            OGN_CORE_ASSERT(instance_api_version, "[Vulkan Context] Couldn't enumerate instance version");

            VkPhysicalDeviceProperties device_properties;
            vkGetPhysicalDeviceProperties(m_PhysicalDevice, &device_properties);

            u32 apiVersionVariant = VK_API_VERSION_VARIANT(instance_api_version);
            u32 apiVersionMajor   = VK_API_VERSION_MAJOR(instance_api_version);
            u32 apiVersionMinor   = VK_API_VERSION_MINOR(instance_api_version);
            u32 apiVersionPatch   = VK_API_VERSION_PATCH(instance_api_version);
            OGN_CORE_INFO("Vulkan Info:");
            OGN_CORE_INFO(" API Version   : {}.{}.{}.{}", apiVersionVariant, apiVersionMajor, apiVersionMinor, apiVersionPatch);
            OGN_CORE_INFO(" Vendor ID     : {}", device_properties.vendorID);
            OGN_CORE_INFO(" Renderer      : {}", device_properties.deviceName);
            OGN_CORE_INFO(" Driver Version: {}", device_properties.driverVersion);
        }
#endif
    }

    void VulkanContext::CreateVkInstance()
    {
#ifdef OGN_VULKAN_API
        u32 extension_count;
        const char **extensions = glfwGetRequiredInstanceExtensions(&extension_count);
        OGN_CORE_ASSERT(extensions != nullptr, "[Vulkan Context] Failed to get required extensions from GLFW");

        OGN_CORE_INFO("[Vulkan Context] GLFW Required Extensions: ");
        for (u32 i = 0; i < extension_count; ++i)
        {
            OGN_CORE_INFO("    {}", extensions[i]);
        }

        // instance create info
        VkInstanceCreateInfo create_info    = {};
        create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.enabledExtensionCount   = extension_count;
        create_info.ppEnabledExtensionNames = extensions;

        const char *validationLayers[] = { "VK_LAYER_KHRONOS_validation" };
        create_info.enabledLayerCount = 0;

#ifdef OGN_DEBUG
        create_info.enabledLayerCount = 1;
        create_info.ppEnabledLayerNames = validationLayers;
#endif

        // check the result of instance creation
        if (VkResult result = vkCreateInstance(&create_info, m_Allocator, &m_Instance); result != VK_SUCCESS)
        {
            OGN_CORE_ERROR("[Vulkan Context] Failed to create Vulkan instance: VkResult = {}", result);
            OGN_CORE_ASSERT(false, "[Vulkan Context] Instance creation failed.");
        }
        else
        {
            OGN_CORE_INFO("[Vulkan Context] Vulkan instance created successfully.");
        }
#endif
    }

    void VulkanContext::CreateVkPhysicalDevice()
    {
#ifdef OGN_VULKAN_API
        u32 gpu_count = 0;
        vkEnumeratePhysicalDevices(m_Instance, &gpu_count, VK_NULL_HANDLE);
        OGN_CORE_ASSERT(gpu_count, "[Vulkan Context] Failed to enumerate physical devices");
        vkEnumeratePhysicalDevices(m_Instance, &gpu_count, &m_PhysicalDevice);

        m_QueueFamily  = std::numeric_limits<u32>::max();
        u32 count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, VK_NULL_HANDLE);
        auto *queue_families = new VkQueueFamilyProperties[count];
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, queue_families);
        OGN_CORE_ASSERT(queue_families, "[Vulkan Context] Queue families is null");

        for (u32 index = 0; index < count; ++index)
        {
            VkQueueFamilyProperties properties = queue_families[index];
            if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                m_QueueFamily = index;
                break;
            }
        }
        OGN_CORE_ASSERT(m_QueueFamily != std::numeric_limits<u32>::max(), "[Vulkan Context] Invalid queue family");
        delete[] queue_families;
#endif
    }

    void VulkanContext::CreateVkSurface()
    {
        VkErrorCheck(glfwCreateWindowSurface(m_Instance, m_WindowHandle, m_Allocator, &m_Surface));
    }

    void VulkanContext::CreateVkDevice()
    {        
#ifdef OGN_VULKAN_API
        // create logical device (with 1 queue)
        const char *device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        // enumerate physical device extension
        u32 properties_count = 0;
        std::vector<VkExtensionProperties> properties;
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &properties_count, nullptr);
        properties.resize(properties_count);
        vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &properties_count, properties.data());

        // specify the queue priorities
        constexpr float queue_priorities[] = {1.0f};

        VkDeviceQueueCreateInfo queue_info[1] = {};
        queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info[0].queueFamilyIndex = m_QueueFamily;
        queue_info[0].queueCount = 1;
        queue_info[0].pQueuePriorities = queue_priorities;
        VkDeviceCreateInfo device_create_info      = {};
        device_create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.queueCreateInfoCount    = std::size(queue_info);
        device_create_info.pQueueCreateInfos       = queue_info;
        device_create_info.enabledExtensionCount   = std::size(device_extensions);
        device_create_info.ppEnabledExtensionNames = device_extensions;

        VkErrorCheck(vkCreateDevice(m_PhysicalDevice, &device_create_info, m_Allocator, &m_Device));

        vkGetDeviceQueue(m_Device, m_QueueFamily, 0, &m_Queue);
        OGN_CORE_INFO("[Vulkan Context] Physical device created successfully");
#endif
    }

    void VulkanContext::CreateVkSwapchain()
    {
#ifdef OGN_VULKAN_API
        const SwapchainSupportDetails swapchain_support_details = QuerySwapchainCapabilities(m_PhysicalDevice, m_Surface);
        const VkSurfaceFormatKHR surface_format         = ChooseSurfaceFormat(swapchain_support_details.Formats);
        const VkPresentModeKHR present_mode             = ChoosePresentMode(swapchain_support_details.PresentModes);

        i32 width, height;
        glfwGetFramebufferSize(m_WindowHandle, &width, &height);
        VkExtent2D extent   = ChooseSwapExtent(swapchain_support_details.Capabilities, width, height);

        VkSwapchainCreateInfoKHR      createInfo{};
        createInfo.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface          = m_Surface;
        createInfo.minImageCount    = m_MinImageCount;
        createInfo.imageFormat      = surface_format.format;
        createInfo.imageColorSpace  = surface_format.colorSpace;
        createInfo.imageExtent      = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        const QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice, m_Surface);
        u32 queue_family_indices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

        if (indices.GraphicsFamily != indices.PresentFamily)
        {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queue_family_indices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform   = swapchain_support_details.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode    = present_mode;
        createInfo.clipped        = VK_TRUE;
        createInfo.oldSwapchain   = VK_NULL_HANDLE;

        const VkResult result = vkCreateSwapchainKHR(m_Device, &createInfo, m_Allocator, &m_Swapchain.Handle);
        OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to create swapchain.");

        u32 imageCount = 0;
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain.Handle, &imageCount, nullptr);
        m_Swapchain.Images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain.Handle, &imageCount, m_Swapchain.Images.data());

        m_Swapchain.ImageFormat = surface_format.format;
        m_Swapchain.Extent = extent;
#endif
    }

    void VulkanContext::CreateVkImageViews()
    {
#ifdef OGN_VULKAN_API
        m_Swapchain.Views.resize(m_Swapchain.Images.size());

        for (size_t i = 0; i < m_Swapchain.Images.size(); ++i)
        {
            VkImageViewCreateInfo                      createInfo{};
            createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image                           = m_Swapchain.Images[i];
            createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format                          = m_Swapchain.ImageFormat;
            createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = 1;

            const VkResult result = vkCreateImageView(m_Device, &createInfo, m_Allocator, &m_Swapchain.Views[i]);
            OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to create image views.");
        }
#endif
    }

    void VulkanContext::CreateVkDescriptorPool()
    {
#ifdef OGN_VULKAN_API
        const VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets       = 1000 * std::size(pool_sizes);
        pool_info.poolSizeCount = static_cast<u32>(std::size(pool_sizes));
        pool_info.pPoolSizes    = pool_sizes;

        VkErrorCheck(vkCreateDescriptorPool(m_Device, &pool_info, m_Allocator, &m_DescriptorPool));
#endif
    }

    void VulkanContext::CreateGraphicsPipeline()
    {
    }
    
}