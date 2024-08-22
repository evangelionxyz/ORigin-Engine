// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanContext.h"

#include <optional>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace origin
{

    // Swapcahin helpers
    struct SwapchainSupportDetails
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> GraphicsFamily;
        std::optional<uint32_t> PresentFamily;

        bool IsComplete() 
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

    static VkExtent2D ChooseSwpaExtent(const VkSurfaceCapabilitiesKHR &capabilities, int width, int height)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices;

        uint32_t familyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.GraphicsFamily = i;
            }

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
            {
                indices.PresentFamily = i;
            }

            if (indices.IsComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    static SwapchainSupportDetails QuerySwapchainCapabilities(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapchainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);
        
        uint32_t formatCount = 0;
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
#ifdef OGN_PLATFORM_LINUX
        for (auto imageView : m_Swapchain.ImageViews)
        {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(m_Device, m_Swapchain.Handle, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyDevice(m_Device, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
#endif
    }

    void VulkanContext::Init(Window *window)
    {
#ifdef OGN_PLATFORM_LINUX
        m_WindowHandle = (GLFWwindow *)window->GetNativeWindow();

        CreateVkInstance();
        CreateVkPhysicalDevice();

        uint32_t instanceApiVersion = 0;
        vkEnumerateInstanceVersion(&instanceApiVersion);
        OGN_CORE_ASSERT(instanceApiVersion, "[Vulkan Context] Couldn't enumerate instance version");

        VkPhysicalDeviceProperties deviceProerties;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProerties);

        uint32_t apiVersionVariant = VK_API_VERSION_VARIANT(instanceApiVersion);
        uint32_t apiVersionMajor   = VK_API_VERSION_MAJOR(instanceApiVersion);
        uint32_t apiVersionMinor   = VK_API_VERSION_MINOR(instanceApiVersion);
        uint32_t apiVersionPatch   = VK_API_VERSION_PATCH(instanceApiVersion);
        OGN_CORE_INFO("Vulkan Info:");
        OGN_CORE_INFO(" API Version   : {}.{}.{}.{}", apiVersionVariant, apiVersionMajor, apiVersionMinor, apiVersionPatch);
        OGN_CORE_INFO(" Vendor ID     : {}", deviceProerties.vendorID);
        OGN_CORE_INFO(" Renderer      : {}", deviceProerties.deviceName);
        OGN_CORE_INFO(" Driver Version: {}", deviceProerties.driverVersion);

        CreateVkSurface();
        CreateVkDevice();
        CreateVkSwapchain();
        CreateVkImageViews();
#endif
    }

    void VulkanContext::CreateVkInstance()
    {
        // application info
        VkApplicationInfo appInfo{};
        appInfo.sType              =  VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.apiVersion         = VK_API_VERSION_1_0;
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.engineVersion      = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pApplicationName   = "ORigin";
        appInfo.pEngineName        = "ORigin Engine";

        uint32_t extensionCount;
        const char **extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
        OGN_CORE_ASSERT(extensions != nullptr, "[Vulkan Context] Failed to get required extensions from GLFW");

        OGN_CORE_INFO("[Vulkan Context] GLFW Required Extensions: ");
        for (uint32_t i = 0; i < extensionCount; ++i)
        {
            OGN_CORE_INFO("    {}", extensions[i]);
        }

        // instance create info
        VkInstanceCreateInfo createInfo{};
        createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo        = &appInfo;
        createInfo.enabledExtensionCount   = extensionCount;
        createInfo.ppEnabledExtensionNames = extensions;

        const char *validationLayers[] = { "VK_LAYER_KHRONOS_validation" };
        createInfo.enabledLayerCount = 0;

#ifdef OGN_DEBUG
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validationLayers;
#endif
        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
        
        // check the result of instance creation
        if (result != VK_SUCCESS)
        {
            OGN_CORE_ERROR("[Vulkan Context] Failed to create Vulkan instance: VkResult = {}", result);
            OGN_CORE_ASSERT(false, "[Vulkan Context] Instance creation failed.");
        }
        else
        {
            OGN_CORE_INFO("[Vulkan Context] Vulkan instance created successfully.");
        }
    }

    void VulkanContext::CreateVkPhysicalDevice()
    {
        uint32_t count = 0;
        vkEnumeratePhysicalDevices(m_Instance, &count, NULL);
        OGN_CORE_ASSERT(count, "[Vulkan Context] Failed to enumerate physical devices");
        vkEnumeratePhysicalDevices(m_Instance, &count, &m_PhysicalDevice);
    }

    void VulkanContext::CreateVkSurface()
    {

#ifdef OGN_PLATFORM_WINDOWS
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
        surfaceCreateInfo.hwnd = glfwGetWin32Window(m_WindowHandle);

        vkCreateWin32SurfaceKHR(m_Instance, &surfaceCreateInfo, nullptr, &m_Surface);
#else
        glfwCreateWindowSurface(m_Instance, m_WindowHandle, nullptr, &m_Surface);
#endif
        m_QueueFamilyIndex = UINT32_MAX;

        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, NULL);

        VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[count];
        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &count, queueFamilies);
        OGN_CORE_ASSERT(queueFamilies, "[Vulkan Context] Queue families is null");

        for (uint32_t index = 0; index < count; ++index)
        {
            VkQueueFamilyProperties properties = queueFamilies[index];
#ifdef OGN_PLATFORM_WINDOWS
            int isSupport = vkGetPhysicalDeviceWin32PresentationSupportKHR(m_PhysicalDevice, index);
#elif
            int isSupport = glfwGetPhysicalDevicePresentationSupport(m_Instance, m_PhysicalDevice, index);
#endif
            if ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && isSupport) 
            {
                m_QueueFamilyIndex = index;
                break;
            }
        }

        OGN_CORE_ASSERT(m_QueueFamilyIndex != UINT32_MAX, "[Vulkan Context] Invalid queue family");
        delete[] queueFamilies;
    }

    void VulkanContext::CreateVkDevice()
    {
        // specify the queue priorites
        float queuePriorities = 1.0f;

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = m_QueueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriorities;


        // specify the device extensions required
        std::vector<const char *> deviceExtensionNames = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        // specify validation layers if in debug mode
        std::vector<const char *> validationLayers;
#ifdef OGN_DEBUG
        validationLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

        // populate the VkDeviceCreateInfo structure
        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensionNames.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensionNames.data();

        // enbale validation layers if any
        if(!validationLayers.empty())
        {
            deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            deviceCreateInfo.enabledLayerCount = 0;
            deviceCreateInfo.ppEnabledLayerNames = nullptr;
        }

        // create the logical device
        VkResult result = vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device);
        OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to create device");

        vkGetDeviceQueue(m_Device, m_QueueFamilyIndex, 0, &m_Queue);

        OGN_CORE_INFO("[Vulkan Context] Physical device created succesfully");
    }

    void VulkanContext::CreateVkSwapchain()
    {
        SwapchainSupportDetails swapchainSupport = QuerySwapchainCapabilities(m_PhysicalDevice, m_Surface);
        VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(swapchainSupport.Formats);
        VkPresentModeKHR presentMode = ChoosePresentMode(swapchainSupport.PresentModes);

        int width, height;
        glfwGetFramebufferSize(m_WindowHandle, &width, &height);
        VkExtent2D extent = ChooseSwpaExtent(swapchainSupport.Capabilities, width, height);

        uint32_t imageCount = swapchainSupport.Capabilities.minImageCount + 1;
        if (swapchainSupport.Capabilities.maxImageCount > 0 && imageCount > swapchainSupport.Capabilities.maxImageCount)
        {
            imageCount = swapchainSupport.Capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice, m_Surface);
        uint32_t queueFamilyIndices[] = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

        if (indices.GraphicsFamily != indices.PresentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapchainSupport.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_Swapchain.Handle);
        OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to create swapchain.");

        vkGetSwapchainImagesKHR(m_Device, m_Swapchain.Handle, &imageCount, nullptr);
        m_Swapchain.Images.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain.Handle, &imageCount, m_Swapchain.Images.data());

        m_Swapchain.ImageFormat = surfaceFormat.format;
        m_Swapchain.Extent = extent;

    }

    void VulkanContext::CreateVkImageViews()
    {
        m_Swapchain.ImageViews.resize(m_Swapchain.Images.size());

        for (size_t i = 0; i < m_Swapchain.Images.size(); ++i)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_Swapchain.Images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_Swapchain.ImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(m_Device, &createInfo, nullptr, &m_Swapchain.ImageViews[i]);
            OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to create image views.");
        }
    }

    void VulkanContext::CreateGraphicsPipeline()
    {
        
    }

    void VulkanContext::Test()
    {
        // query surface capabilities
        VkSurfaceCapabilitiesKHR capabilities;
        VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &capabilities);
        OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to get surface capabilities");

        // query surface formats
        uint32_t formatCount = 0;
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, nullptr);
        OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to get surface formats");

        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, formats.data());
        OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to get surface formats");

        // select the preferred surface format
        VkSurfaceFormatKHR choosenFormat = formats[0];
        for (const auto &format : formats)
        {
            if (format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && format.format == VK_FORMAT_B8G8R8A8_SRGB)
            {
                choosenFormat = format;
                break;
            }
        }

        // query present modes
        uint32_t presentModeCount = 0;
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, nullptr);
        OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to get present modes count");

        // select the preferred present mode
        VkPresentModeKHR choosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;
        if (presentModeCount)
        {
            std::vector<VkPresentModeKHR> presentModes(presentModeCount);
            result  = vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, presentModes.data());
            OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to get present modes");
            for (const auto &mode : presentModes)
            {
                if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    choosenPresentMode = mode;
                    break;
                }
            }
        }

        // prepare the swapchain create info
        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = m_Surface;
        swapchainCreateInfo.minImageCount = capabilities.minImageCount;
        swapchainCreateInfo.imageFormat = choosenFormat.format;
        swapchainCreateInfo.imageColorSpace = choosenFormat.colorSpace;
        swapchainCreateInfo.imageExtent = capabilities.currentExtent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCreateInfo.queueFamilyIndexCount = 1;
            swapchainCreateInfo.pQueueFamilyIndices = &m_QueueFamilyIndex;
        swapchainCreateInfo.preTransform = capabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = choosenPresentMode;
        swapchainCreateInfo.clipped = VK_TRUE;

        // set the oldSwapchain handle if it is valid, otherwise use VK_NULL_HANDLE
        if(m_Swapchain.Handle != VK_NULL_HANDLE)
        {
            swapchainCreateInfo.oldSwapchain = m_Swapchain.Handle;
        }

        // create new swapchain
        VkSwapchainKHR newSwapchain;
        result = vkCreateSwapchainKHR(m_Device, &swapchainCreateInfo, nullptr, &newSwapchain);
        OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to create swapchain");


        // destroy old swapchain resources (only if not null)
        if (m_Swapchain.Handle != VK_NULL_HANDLE)
        {
            for (auto imageView : m_Swapchain.ImageViews)
            {
                vkDestroyImageView(m_Device, imageView, nullptr);
            }
            vkDestroySwapchainKHR(m_Device, m_Swapchain.Handle, nullptr);
        }
        
        // replace the old swapchain with new one
        m_Swapchain.Handle = newSwapchain;

        // retrieve the swapchain images
        uint32_t swapchainImageCount = 0;
        result = vkGetSwapchainImagesKHR(m_Device, m_Swapchain.Handle, &swapchainImageCount, nullptr);
        OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to get swapchain image count");

        m_Swapchain.Images.resize(swapchainImageCount);
        vkGetSwapchainImagesKHR(m_Device, m_Swapchain.Handle, &swapchainImageCount, m_Swapchain.Images.data());
        OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to get swapchain images");

        // create image views for the swapchain images
        VkImageSubresourceRange subResourceRange{};
        subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subResourceRange.baseMipLevel = 0;
        subResourceRange.levelCount = 1;
        subResourceRange.baseArrayLayer = 0;
        subResourceRange.layerCount = 1;

        VkComponentMapping componentMapping{};

        for (uint32_t i = 0; i < swapchainImageCount; ++i)
        {
            VkImageViewCreateInfo imgViewCreateInfo{};
            imgViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imgViewCreateInfo.image = m_Swapchain.Images[i];
            imgViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imgViewCreateInfo.format = choosenFormat.format;
            imgViewCreateInfo.components = componentMapping;
            imgViewCreateInfo.subresourceRange = subResourceRange;

            result = vkCreateImageView(m_Device, &imgViewCreateInfo, nullptr, &m_Swapchain.ImageViews[i]);
            OGN_CORE_ASSERT(result == VK_SUCCESS, "[Vulkan Context] Failed to create image view {}", i);
        }
    }

}