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
    FreeCommandBuffers();

    DestroyFramebuffers();

    // reset command pool
    ResetCommandPool();

    // destroy render pass
    vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, m_Allocator);

    // destroy pipeline
    m_GraphicsPipeline.Cleanup();

    // destroy descriptor pool
    vkDestroyDescriptorPool(m_LogicalDevice, m_DescriptorPool, m_Allocator);

    // destroy command pool
    vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, m_Allocator);

    // destroy semaphores
    m_Queue.Destroy();

    // destroy swapchain
    m_Swapchain.Destroy();

    // destroy surface
    vkDestroySurfaceKHR(m_Instance, m_Surface, m_Allocator);

#ifdef OGN_DEBUG
    // destroy messenger
    const auto dbg_messenger_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
    OGN_CORE_ASSERT(dbg_messenger_func, "[Vulkan] Could not find address of 'vkDestroyDebugUtilsMessengerEXT'");
    dbg_messenger_func(m_Instance, m_DebugMessenger, m_Allocator);
#endif

    // destroy device
    vkDestroyDevice(m_LogicalDevice, m_Allocator);

    // destroy instance
    vkDestroyInstance(m_Instance, m_Allocator);
}

void VulkanContext::Init(Window *window)
{
    m_WindowHandle = window->GetNativeWindow();
    m_ClearValue.color.float32[0] = 0.1f;
    m_ClearValue.color.float32[1] = 0.1f;
    m_ClearValue.color.float32[2] = 0.1f;
    m_ClearValue.color.float32[3] = 1.0f;

    CreateInstance();
#ifdef OGN_DEBUG
    CreateDebugCallback();
#endif
    CreateWindowSurface();

    m_PhysicalDevice = VulkanPhysicalDevice(m_Instance, m_Surface);
    m_QueueFamily = m_PhysicalDevice.SelectDevice(VK_QUEUE_GRAPHICS_BIT, true);

    CreateDevice();
    CreateSwapchain();
    CreateRenderPass();
    CreateCommandPool();

    m_Queue = VulkanQueue(m_LogicalDevice, m_Allocator, m_QueueFamily, 0);
    CreateDescriptorPool();

    // Print info
    {
        u32 instance_api_version = 0;
        vkEnumerateInstanceVersion(&instance_api_version);
        OGN_CORE_ASSERT(instance_api_version, "[Vulkan Context] Couldn't enumerate instance version");

        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice.GetSelectedDevice().Device, &device_properties);
        u32 apiVersionVariant = VK_API_VERSION_VARIANT(instance_api_version);
        u32 apiVersionMajor = VK_API_VERSION_MAJOR(instance_api_version);
        u32 apiVersionMinor = VK_API_VERSION_MINOR(instance_api_version);
        u32 apiVersionPatch = VK_API_VERSION_PATCH(instance_api_version);
        OGN_CORE_INFO("Vulkan Info:");
        OGN_CORE_INFO(" API Version   : {}.{}.{}.{}", apiVersionVariant, apiVersionMajor, apiVersionMinor, apiVersionPatch);
        OGN_CORE_INFO(" Vendor ID     : {}", device_properties.vendorID);
        OGN_CORE_INFO(" Renderer      : {}", device_properties.deviceName);
        OGN_CORE_INFO(" Driver Version: {}", device_properties.driverVersion);
    }

    m_GraphicsPipeline = VulkanGraphicsPipeline(m_LogicalDevice, m_RenderPass, m_Allocator);

    CreateGraphicsPipeline();
    CreateFramebuffers();

    CreateCommandBuffers();
}

void VulkanContext::ResetCommandPool()
{
    m_Queue.WaitIdle();
    VK_ERROR_CHECK(vkResetCommandPool(m_LogicalDevice, m_CommandPool, 0),
        "[Vulkan] Failed to reset command pool");
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
    const char **req_extension = glfwGetRequiredInstanceExtensions(&req_extension_count);
    std::vector<const char *> extensions;
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

    const char *layers[] = { "VK_LAYER_KHRONOS_validation" };
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

void VulkanContext::CreateDebugCallback()
{
    VkDebugUtilsMessengerCreateInfoEXT msg_create_info = {};
    msg_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    msg_create_info.pNext = VK_NULL_HANDLE;
    msg_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    msg_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    msg_create_info.pfnUserCallback = VkDebugMessengerCallback;
    msg_create_info.pUserData = VK_NULL_HANDLE;

    const auto dbg_messasger_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
    OGN_CORE_ASSERT(dbg_messasger_func, "[Vulkan] Cannot find address of vkCreateDebugUtilsMessengerEXT");

    const VkResult result = dbg_messasger_func(m_Instance, &msg_create_info, m_Allocator, &m_DebugMessenger);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create debug messenger");

}

void VulkanContext::CreateWindowSurface()
{
    VK_ERROR_CHECK(glfwCreateWindowSurface(m_Instance, m_WindowHandle, m_Allocator, &m_Surface), "[Vulkan] Failed to crreate window surface");
}

void VulkanContext::CreateDevice()
{
    const float queue_priorities[] = { 1.0f };
    VkDeviceQueueCreateInfo queue_create_info = {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.pQueuePriorities = queue_priorities;
    queue_create_info.queueFamilyIndex = m_QueueFamily;
    queue_create_info.queueCount = 1;

    const char *device_extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME };

    if (m_PhysicalDevice.GetSelectedDevice().Features.geometryShader == VK_FALSE)
    {
        OGN_CORE_WARN("[Vulkan] Geometry shader is not supported");
    }
    if (m_PhysicalDevice.GetSelectedDevice().Features.tessellationShader == VK_FALSE)
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

    const VkResult result = vkCreateDevice(m_PhysicalDevice.GetSelectedDevice().Device, &create_info, m_Allocator, &m_LogicalDevice);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create logical device");
    OGN_CORE_INFO("[Vulkan Context] Physical device created successfully");
}

void VulkanContext::CreateSwapchain()
{
    i32 width, height;
    glfwGetFramebufferSize(m_WindowHandle, &width, &height);

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice.GetSelectedDevice().Device, m_Surface, &capabilities);

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

    const std::vector<VkPresentModeKHR> &present_modes = m_PhysicalDevice.GetSelectedDevice().PresentModes;
    const VkPresentModeKHR present_mode = VkChoosePresentMode(present_modes);

    VkSurfaceFormatKHR format = VkChooseSurfaceFormat(m_PhysicalDevice.GetSelectedDevice().SurfaceFormats);
    VkImageUsageFlags image_usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    m_Swapchain = VulkanSwapchain(m_LogicalDevice, m_Allocator, m_Surface,
        format, capabilities, present_mode, image_usage, m_QueueFamily);
}

void VulkanContext::CreateCommandPool()
{
    VkCommandPoolCreateInfo pool_create_info = {};
    pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_create_info.queueFamilyIndex = m_QueueFamily;

    VK_ERROR_CHECK(vkCreateCommandPool(m_LogicalDevice, &pool_create_info, m_Allocator, &m_CommandPool),
        "[Vulkan] Failed to create command pool");
}

void VulkanContext::CreateDescriptorPool()
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
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * std::size(pool_sizes);
    pool_info.poolSizeCount = std::size(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    VK_ERROR_CHECK(vkCreateDescriptorPool(m_LogicalDevice, &pool_info, m_Allocator, &m_DescriptorPool),
        "[Vulkan] Failed to create command pool");
}

void VulkanContext::CreateGraphicsPipeline()
{
    Ref<VulkanShader> shader = CreateRef<VulkanShader>("Resources/Shaders/Vulkan/default.glsl", true);

    VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexAttributeDescriptionCount = 0;
    vertex_input_info.vertexBindingDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
    input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterization_info = {};
    rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_info.depthClampEnable = VK_FALSE;
    rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_info.lineWidth = 1.0f;
    rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterization_info.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisample_info = {};
    multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_info.sampleShadingEnable = VK_FALSE;
    multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo color_blend_info = {};
    color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_info.logicOpEnable = VK_FALSE;
    color_blend_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_info.attachmentCount = 1;
    color_blend_info.pAttachments = &color_blend_attachment;
    color_blend_info.blendConstants[0] = 0.0f;
    color_blend_info.blendConstants[1] = 0.0f;
    color_blend_info.blendConstants[2] = 0.0f;
    color_blend_info.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pushConstantRangeCount = 0;

    VkViewport viewport = { 0.0f, 0.0f,
        static_cast<float>(m_Swapchain.GetVkExtent2D().width),
        static_cast<float>(m_Swapchain.GetVkExtent2D().height),
        0.0f, 1.0f
    };
    VkRect2D scissor = { {0, 0}, m_Swapchain.GetVkExtent2D() };

    m_GraphicsPipeline.Create(shader->GetShaderStages(),
        vertex_input_info, input_assembly_info, viewport, scissor,
        rasterization_info, multisample_info, color_blend_info,
        pipeline_layout_info);
}

void VulkanContext::CreateRenderPass()
{
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = m_Swapchain.GetVkFormat().format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pInputAttachments = VK_NULL_HANDLE;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = VK_NULL_HANDLE;
    subpass.pResolveAttachments = VK_NULL_HANDLE;
    subpass.pDepthStencilAttachment = VK_NULL_HANDLE;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    VkSubpassDependency subpass_dependency = {};
    subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpass_dependency.dstSubpass = 0;
    subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.srcAccessMask = 0;
    subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info = {};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &subpass_dependency;

    VK_ERROR_CHECK(vkCreateRenderPass(m_LogicalDevice, &render_pass_info, m_Allocator, &m_RenderPass),
        "[Vulkan] Failed to create render pass");
}

void VulkanContext::CreateFramebuffers()
{
    const u32 width = m_Swapchain.GetVkExtent2D().width;
    const u32 height = m_Swapchain.GetVkExtent2D().height;

    const u32 image_count = static_cast<u32>(m_Swapchain.GetVkImageCount());
    m_Framebuffers.resize(image_count);

    for (u32 i = 0; i < image_count; i++)
    {
        VkImageView attachments[] = { m_Swapchain.GetVkImageView(i) };
        VkFramebufferCreateInfo framebuffer_create_info = {};
        framebuffer_create_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.renderPass      = m_RenderPass;
        framebuffer_create_info.width           = width;
        framebuffer_create_info.height          = height;
        framebuffer_create_info.layers          = 1;
        framebuffer_create_info.pAttachments    = attachments;
        framebuffer_create_info.attachmentCount = std::size(attachments);

        VkResult res = vkCreateFramebuffer(m_LogicalDevice, &framebuffer_create_info, m_Allocator, &m_Framebuffers[i]);
        VK_ERROR_CHECK(res, "[Vulkan] Failed to create framebuffer");
    }
}

void VulkanContext::CreateCommandBuffers()
{
    const u32 image_count = m_Swapchain.GetVkImageCount();
    m_CommandBuffers.resize(image_count);

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandBufferCount = image_count;
    alloc_info.commandPool        = m_CommandPool;
    alloc_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkResult res = vkAllocateCommandBuffers(m_LogicalDevice, &alloc_info, m_CommandBuffers.data());
    VK_ERROR_CHECK(res, "[Vulkan] Failed to create command buffers");
}

void VulkanContext::FreeCommandBuffers()
{
    vkDeviceWaitIdle(m_LogicalDevice);

    const u32 count = static_cast<u32>(m_CommandBuffers.size());
    vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, count, m_CommandBuffers.data());
}

void VulkanContext::DestroyFramebuffers()
{
    for (const auto fb : m_Framebuffers)
        vkDestroyFramebuffer(m_LogicalDevice, fb, m_Allocator);
}

void VulkanContext::RecreateSwapchain()
{
    vkDeviceWaitIdle(m_LogicalDevice);

    DestroyFramebuffers();

    m_Swapchain.Destroy();

    CreateSwapchain();

    const u32 width = m_Swapchain.GetVkExtent2D().width;
    const u32 height = m_Swapchain.GetVkExtent2D().height;
    m_GraphicsPipeline.Resize(width, height);

    CreateFramebuffers();
}

void VulkanContext::Present()
{
    m_Queue.WaitIdle();

    u32 image_index = 0;
    VkResult result = vkAcquireNextImageKHR(m_LogicalDevice, m_Swapchain.GetVkSwapchain(), UINT64_MAX, m_Queue.GetSemaphore(), VK_NULL_HANDLE, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swap chain image");
    }

    m_Queue.WaitAndResetFences();

    vkResetCommandBuffer(m_CommandBuffers[image_index], 0);
    RecordCommandBuffer(m_CommandBuffers[image_index], image_index);

    m_Queue.SubmitAsync(m_CommandBuffers[image_index]);

    result = m_Queue.Present(image_index, m_Swapchain.GetVkSwapchain());
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        RecreateSwapchain();
    }
}

void VulkanContext::RecordCommandBuffer(VkCommandBuffer command_buffer, u32 image_index)
{
    const u32 width = m_Swapchain.GetVkExtent2D().width;
    const u32 height = m_Swapchain.GetVkExtent2D().height;

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    begin_info.pNext            = VK_NULL_HANDLE;
    begin_info.pInheritanceInfo = VK_NULL_HANDLE;
    vkBeginCommandBuffer(command_buffer, &begin_info);

    m_GraphicsPipeline.BeginRenderPass(command_buffer, m_Framebuffers[image_index], m_ClearValue, width, height);

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline.GetPipeline());

    VkViewport vp = m_GraphicsPipeline.GetViewport();
    VkRect2D scissor = m_GraphicsPipeline.GetScissor();

    vkCmdSetViewport(command_buffer, 0, 1, &vp);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);

    vkCmdDraw(command_buffer, 3, 1, 0, 0);

    // record dear imgui primitives into command buffer
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);

    m_GraphicsPipeline.EndRenderPass(command_buffer);

    vkEndCommandBuffer(command_buffer); // !command buffer
}

}