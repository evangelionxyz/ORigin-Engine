// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanContext.hpp"
#include "VulkanWrapper.hpp"
#include "VulkanShader.hpp"

#ifdef OGN_PLATFORM_WINDOWS
#   define VK_USE_PLATFORM_WIN32_KHR
#   include <GLFW/glfw3native.h>
#endif

#include <backends/imgui_impl_vulkan.h>

#include "Origin/Core/Window.h"

namespace origin {

VulkanContext* VulkanContext::s_Instance = nullptr;

VulkanContext::VulkanContext()
{
    s_Instance = this;
}

VulkanContext* VulkanContext::GetInstance()
{
    return s_Instance;
}

void VulkanContext::Shutdown()
{
    FreeCommandBuffers();

    DestroyFramebuffers();

    // reset command pool
    ResetCommandPool();

    // destroy render pass
    m_render_pass->Destroy();

    // destroy descriptor pool
    vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);

    // destroy command pool
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);

    // destroy semaphores
    m_queue.Destroy();

    // destroy swapchain
    m_swapchain.Destroy();

    // destroy surface
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

#ifdef OGN_DEBUG
    // destroy messenger
    const auto dbg_messenger_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));
    OGN_CORE_ASSERT(dbg_messenger_func, "[Vulkan] Could not find address of 'vkDestroyDebugUtilsMessengerEXT'");
    dbg_messenger_func(m_instance, m_debug_messenger, nullptr);
#endif

    // destroy device
    vkDestroyDevice(m_device, nullptr);

    // destroy instance
    vkDestroyInstance(m_instance, nullptr);
}

void VulkanContext::Init(Window* window)
{
    m_window_handle = window->GetNativeWindow();

    CreateInstance();
#ifdef OGN_DEBUG
    CreateDebugCallback();
#endif
    CreateWindowSurface();

    m_physical_device = VulkanPhysicalDevice(m_instance, m_surface);
    m_queue_family = m_physical_device.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);

    CreateDevice();
    CreateSwapchain();
    CreateRenderPass();
    CreateCommandPool();

    m_queue = VulkanQueue(m_device, m_queue_family, 0);
    CreateDescriptorPool();

    // Print info
    u32 instance_api_version = 0;
    vkEnumerateInstanceVersion(&instance_api_version);
    OGN_CORE_ASSERT(instance_api_version, "[Vulkan Context] Couldn't enumerate instance version");

    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(m_physical_device.GetSelectedDevice().Device, &device_properties);
    u32 apiVersionVariant = VK_API_VERSION_VARIANT(instance_api_version);
    u32 apiVersionMajor = VK_API_VERSION_MAJOR(instance_api_version);
    u32 apiVersionMinor = VK_API_VERSION_MINOR(instance_api_version);
    u32 apiVersionPatch = VK_API_VERSION_PATCH(instance_api_version);
    OGN_CORE_INFO("Vulkan Info:");
    OGN_CORE_INFO(" API Version   : {}.{}.{}.{}", apiVersionVariant, apiVersionMajor, apiVersionMinor, apiVersionPatch);
    OGN_CORE_INFO(" Vendor ID     : {}", device_properties.vendorID);
    OGN_CORE_INFO(" Renderer      : {}", device_properties.deviceName);
    OGN_CORE_INFO(" Driver Version: {}", device_properties.driverVersion);

    CreateFramebuffers();
    CreateCommandBuffers();
}

void VulkanContext::ResetCommandPool()
{
    m_queue.WaitIdle();
    VK_ERROR_CHECK(vkResetCommandPool(m_device, m_command_pool, 0), "[Vulkan] Failed to reset command pool");
}

void VulkanContext::CreateInstance()
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
    VkInstanceCreateInfo create_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = static_cast<u32>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();
    create_info.enabledLayerCount = std::size(layers);
    create_info.ppEnabledLayerNames = layers;

    const VkResult res = vkCreateInstance(&create_info, nullptr, &m_instance);
    VK_ERROR_CHECK(res, "[Vulkan] Failed to create instance");
    OGN_CORE_INFO("[Vulkan] Vulkan instance created");

}

void VulkanContext::CreateDebugCallback()
{
    VkDebugUtilsMessengerCreateInfoEXT msg_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    msg_create_info.pNext = VK_NULL_HANDLE;
    msg_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    msg_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    msg_create_info.pfnUserCallback = VkDebugMessengerCallback;
    msg_create_info.pUserData = VK_NULL_HANDLE;

    const auto dbg_messasger_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
    OGN_CORE_ASSERT(dbg_messasger_func, "[Vulkan] Cannot find address of vkCreateDebugUtilsMessengerEXT");

    const VkResult result = dbg_messasger_func(m_instance, &msg_create_info, nullptr, &m_debug_messenger);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create debug messenger");

}

void VulkanContext::CreateWindowSurface()
{
    VK_ERROR_CHECK(glfwCreateWindowSurface(m_instance, m_window_handle, nullptr, &m_surface), "[Vulkan] Failed to crreate window surface");
}

void VulkanContext::CreateDevice()
{
    const float queue_priorities[] = { 1.0f };
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.pQueuePriorities = queue_priorities;
    queue_create_info.queueFamilyIndex = m_queue_family;
    queue_create_info.queueCount = 1;

    const char* device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME };

    if (m_physical_device.GetSelectedDevice().Features.geometryShader == VK_FALSE)
    {
        OGN_CORE_WARN("[Vulkan] Geometry shader is not supported");
    }
    if (m_physical_device.GetSelectedDevice().Features.tessellationShader == VK_FALSE)
    {
        OGN_CORE_WARN("[Vulkan] Tesselation shader is not supported");

    }

    VkPhysicalDeviceFeatures device_features = { 0 };
    device_features.geometryShader = VK_TRUE;
    device_features.tessellationShader = VK_TRUE;

    VkDeviceCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.flags = 0;
    create_info.queueCreateInfoCount = 1;
    create_info.pQueueCreateInfos = &queue_create_info;
    create_info.pNext = VK_NULL_HANDLE;
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = VK_NULL_HANDLE;
    create_info.enabledExtensionCount = std::size(device_extensions);
    create_info.ppEnabledExtensionNames = device_extensions;
    create_info.pEnabledFeatures = &device_features;

    const VkResult result = vkCreateDevice(m_physical_device.GetSelectedDevice().Device, &create_info, nullptr, &m_device);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create logical device");
    OGN_CORE_INFO("[Vulkan Context] Physical device created successfully");
}

void VulkanContext::CreateSwapchain()
{
    i32 width, height;
    glfwGetFramebufferSize(m_window_handle, &width, &height);

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device.GetSelectedDevice().Device, m_surface, &capabilities);

    const VkExtent2D swapchain_extent = {
        static_cast<u32>(width),
        static_cast<u32>(height)
    };

    capabilities.currentExtent.width = std::clamp(
        swapchain_extent.width,
        capabilities.minImageExtent.width,
        capabilities.maxImageExtent.width
    );

    capabilities.currentExtent.height = std::clamp(
        swapchain_extent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height
    );

    const std::vector<VkPresentModeKHR>& present_modes = m_physical_device.GetSelectedDevice().PresentModes;
    const VkPresentModeKHR present_mode = VkChoosePresentMode(present_modes);

    VkSurfaceFormatKHR format = VkChooseSurfaceFormat(m_physical_device.GetSelectedDevice().SurfaceFormats);
    VkImageUsageFlags image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    m_swapchain = VulkanSwapchain(m_device, m_surface, format, capabilities, present_mode, image_usage, m_queue_family);
}

void VulkanContext::CreateCommandPool()
{
    VkCommandPoolCreateInfo pool_create_info = {};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_create_info.queueFamilyIndex = m_queue_family;

    VK_ERROR_CHECK(vkCreateCommandPool(m_device, &pool_create_info, nullptr, &m_command_pool),
                   "[Vulkan] Failed to create command pool");
}

void VulkanContext::CreateDescriptorPool()
{
    const VkDescriptorPoolSize pool_sizes[] =
    {
    { VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100 },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * std::size(pool_sizes);
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VK_ERROR_CHECK(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &m_descriptor_pool),
                   "[Vulkan] Failed to create command pool");
}

void VulkanContext::CreateRenderPass()
{
    VkAttachmentDescription attachment = {};
    attachment.flags = 0;
    attachment.format = m_swapchain.GetVkFormat().format;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    std::vector<VkAttachmentDescription> attachments = { attachment };

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.preserveAttachmentCount = 0;
    subpass.pInputAttachments = VK_NULL_HANDLE;
    subpass.pPreserveAttachments = VK_NULL_HANDLE;
    subpass.pResolveAttachments = VK_NULL_HANDLE;
    subpass.pDepthStencilAttachment = VK_NULL_HANDLE;

    std::vector<VkSubpassDescription> subpasses = { subpass };

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    std::vector<VkSubpassDependency> dependencies = { dependency };

    VkRenderPassCreateInfo render_pass_ci = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
    render_pass_ci.pAttachments = attachments.data();
    render_pass_ci.attachmentCount = static_cast<u32>(attachments.size());
    render_pass_ci.pSubpasses = subpasses.data();
    render_pass_ci.subpassCount = static_cast<u32>(subpasses.size());
    render_pass_ci.pDependencies = dependencies.data();
    render_pass_ci.dependencyCount = static_cast<u32>(dependencies.size());

    m_render_pass = CreateRef<VulkanRenderPass>(attachments, subpasses, dependencies);
}

void VulkanContext::CreateFramebuffers()
{
    const u32 image_count = static_cast<u32>(m_swapchain.GetVkImageCount());
    m_framebuffers.resize(image_count);

    for (u32 i = 0; i < image_count; i++)
    {
        VkImageView attachments[] = { m_swapchain.GetVkImageView(i) };
        VkFramebufferCreateInfo framebuffer_create_info = {};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.renderPass = m_render_pass->GetRenderPass();
        framebuffer_create_info.width = m_swapchain.GetVkExtent2D().width;
        framebuffer_create_info.height = m_swapchain.GetVkExtent2D().height;
        framebuffer_create_info.layers = 1;
        framebuffer_create_info.pAttachments = attachments;
        framebuffer_create_info.attachmentCount = std::size(attachments);

        VkResult res = vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_framebuffers[i]);
        VK_ERROR_CHECK(res, "[Vulkan] Failed to create framebuffer");
    }
}

void VulkanContext::CreateCommandBuffers()
{
    const u32 image_count = m_swapchain.GetVkImageCount();
    m_command_buffers.resize(image_count);

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandBufferCount = image_count;
    alloc_info.commandPool = m_command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkResult res = vkAllocateCommandBuffers(m_device, &alloc_info, m_command_buffers.data());
    VK_ERROR_CHECK(res, "[Vulkan] Failed to create command buffers");
}

void VulkanContext::FreeCommandBuffers()
{
    vkDeviceWaitIdle(m_device);

    const u32 count = static_cast<u32>(m_command_buffers.size());
    vkFreeCommandBuffers(m_device, m_command_pool, count, m_command_buffers.data());
}

void VulkanContext::DestroyFramebuffers()
{
    for (const auto fb : m_framebuffers)
        vkDestroyFramebuffer(m_device, fb, nullptr);
}

void VulkanContext::RecreateSwapchain()
{
    vkDeviceWaitIdle(m_device);
    DestroyFramebuffers();
    m_swapchain.Destroy();
    CreateSwapchain();
    CreateFramebuffers();
}

void VulkanContext::Present()
{
    BeginFrame();
    EndFrame();
}

void VulkanContext::BeginFrame()
{
    m_queue.WaitIdle();
    VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain.GetVkSwapchain(),
                                            UINT64_MAX, m_queue.GetSemaphore(), VK_NULL_HANDLE, &m_current_image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    m_queue.WaitAndResetFences();

    vkResetCommandBuffer(m_command_buffers[m_current_image_index], 0);
    RecordCommandBuffer(m_command_buffers[m_current_image_index], m_current_image_index);
}

void VulkanContext::EndFrame()
{
    m_queue.SubmitAsync(m_command_buffers[m_current_image_index]);

    VkResult result = m_queue.Present(m_current_image_index, m_swapchain.GetVkSwapchain());
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        RecreateSwapchain();
    }
}

void VulkanContext::RecordCommandBuffer(VkCommandBuffer cmd, u32 image_index)
{
    const u32 width = m_swapchain.GetVkExtent2D().width;
    const u32 height = m_swapchain.GetVkExtent2D().height;

    VkCommandBufferBeginInfo begin_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    begin_info.pNext = VK_NULL_HANDLE;
    begin_info.pInheritanceInfo = VK_NULL_HANDLE;
    vkBeginCommandBuffer(cmd, &begin_info);

    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = m_swapchain.GetVkExtent2D();

    VkViewport viewport;
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = static_cast<f32>(width);
    viewport.height = static_cast<f32>(height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    const VkRect2D render_area = { { 0, 0 },{ width, height } };
    m_render_pass->Begin(cmd, render_area, m_framebuffers[image_index], VkClearValue{ 0.1f, 0.1f, 0.1f, 1.0f });

    for (auto& command : m_CommandCallbacks)
    {
        command(cmd, m_framebuffers[image_index], image_index);
    }

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

    m_render_pass->End(cmd);

    vkEndCommandBuffer(cmd); // !command buffer

    //m_CommandCallbacks.clear();
}

void VulkanContext::Submit(CommandCallback callback)
{
    m_CommandCallbacks.push_back(callback);
}

}