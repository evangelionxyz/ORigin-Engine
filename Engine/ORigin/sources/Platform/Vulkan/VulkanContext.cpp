// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanContext.h"
#include "VulkanWrapper.h"

#ifdef OGN_PLATFORM_WINDOWS
#   define VK_USE_PLATFORM_WIN32_KHR
#   include <GLFW/glfw3native.h>
#endif

#include "Origin/Core/Window.h"

namespace origin
{
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
        vkDestroyDescriptorPool(m_Device, m_DescriptorPool, m_Allocator);

        m_Swapchain.Destroy(m_Device, m_Allocator);

        vkDestroySwapchainKHR(m_Device, *m_Swapchain.GetVkSwapchain(), m_Allocator);
        vkDestroySurfaceKHR(m_Instance, m_Surface, m_Allocator);
        vkDestroyDevice(m_Device, m_Allocator);
        vkDestroyInstance(m_Instance, m_Allocator);
    }

    void VulkanContext::Init(Window *window)
    {
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

        CreateVkSwapchain();
        CreateVkImageViews();

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
    }

    void VulkanContext::CreateVkInstance()
    {
        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "ORiginEngine";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Vulkan Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);

        u32 req_extension_count = 0;
        const char** req_extension = glfwGetRequiredInstanceExtensions(&req_extension_count);
        std::vector<const char*> extensions;
        extensions.reserve(req_extension_count);

        for (u32 i = 0; i < req_extension_count; ++i)
            extensions.push_back(req_extension[i]);

        extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#ifdef _WIN32
        extensions.push_back("VK_KHR_win32_surface");
#elif __linux__
        extensions.push_back("VK_KHR_xcb_surface");
#endif
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
        VkInstanceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = static_cast<u32>(extensions.size());
        create_info.ppEnabledExtensionNames = extensions.data();
        create_info.enabledLayerCount = std::size(layers);
        create_info.ppEnabledLayerNames = layers;

        const VkResult res = vkCreateInstance(&create_info, m_Allocator, &m_Instance);
        VK_ERROR_CHECK(res, "[Vulkan] Failed to create instance");
        OGN_CORE_INFO("[Vulkan] Vulkan instance created");

    }

    void VulkanContext::CreateVkPhysicalDevice()
    {
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
    }

    void VulkanContext::CreateVkSurface()
    {
        VK_ERROR_CHECK(glfwCreateWindowSurface(m_Instance, m_WindowHandle, m_Allocator, &m_Surface), "[Vulkan] Failed to crreate window surface");
    }

    void VulkanContext::CreateVkDevice()
    {
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

        //VK_ERROR_CHECK(vkCreateDevice(m_PhysicalDevice, &device_create_info, m_Allocator, &m_Device));

        vkGetDeviceQueue(m_Device, m_QueueFamily, 0, &m_Queue);
        OGN_CORE_INFO("[Vulkan Context] Physical device created successfully");
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
        //m_Swapchain = VulkanSwapchain(m_Device, m_Allocator, m_Surface, );
    }

    void VulkanContext::CreateVkDescriptorPool()
    {
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

        //VK_ERROR_CHECK(vkCreateDescriptorPool(m_Device, &pool_info, m_Allocator, &m_DescriptorPool));
    }

    void VulkanContext::CreateGraphicsPipeline()
    {
    }
    
}