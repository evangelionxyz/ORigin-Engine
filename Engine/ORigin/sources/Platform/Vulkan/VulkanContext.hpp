// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef VULKAN_CONTEXT_HPP
#define VULKAN_CONTEXT_HPP

#include "Origin/Renderer/GraphicsContext.h"
#include "VulkanSwapchain.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "VulkanQueue.hpp"
#include "VulkanRenderPass.hpp"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace origin {

class VulkanContext : public GraphicsContext
{
public:
    VulkanContext();
    void Init(Window *window) override;
    void Shutdown() override;

    static VulkanContext *GetInstance();

    u32 GetVkQueueFamily() { return m_queue_family; }
    const u32 GetMinImageCount() const { return m_min_image_count; }
    VulkanSwapchain *GetSwapchain() { return &m_swapchain; }
    VkDevice GetVkDevice() { return m_device; }
    VkInstance GetVkInstance() { return m_instance; }
    VkPhysicalDevice GetVkPhysicalDevice() { return m_physical_device.GetSelectedDevice().Device; }
    VkQueue GetVkQueue() { return m_queue.GetVkQueue(); }
    VulkanQueue *GetQueue() { return &m_queue; }
    VkSurfaceKHR GetVkSurface() { return m_surface; }
    VkDescriptorPool GetVkDescriptorPool() { return m_descriptor_pool; }
    VkRenderPass GetVkRenderPass() const { return m_render_pass->GetRenderPass(); }
    VkFramebuffer GetFramebuffer(u32 index) { return m_framebuffers[index]; }
    VkCommandPool GetVkCommandPool() { return m_command_pool; }
    void Present();
    
    using CommandCallback = std::function<void(VkCommandBuffer, VkFramebuffer, u32)>;
    void Submit(CommandCallback callback);

private:
    void ResetCommandPool();
    void CreateInstance();
    void CreateDebugCallback();
    void CreateWindowSurface();
    void CreateDevice();
    void CreateSwapchain();
    void CreateCommandPool();
    void CreateDescriptorPool();
    void CreateRenderPass();

    void CreateFramebuffers();
    void CreateCommandBuffers();
    void FreeCommandBuffers();
    void DestroyFramebuffers();
    void RecreateSwapchain();
    void RecordCommandBuffer(VkCommandBuffer command_buffer, u32 image_index);
    void BeginFrame();
    void EndFrame();

    std::vector<CommandCallback> m_CommandCallbacks;

    // Context
    VkInstance             m_instance = VK_NULL_HANDLE;
    VkSurfaceKHR           m_surface = VK_NULL_HANDLE;
    VkDevice               m_device = VK_NULL_HANDLE;
    VkCommandPool          m_command_pool = VK_NULL_HANDLE;
    VkDescriptorPool       m_descriptor_pool = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;
    GLFWwindow            *m_window_handle = nullptr;
    VulkanPhysicalDevice   m_physical_device;
    VulkanQueue            m_queue;
    VulkanSwapchain        m_swapchain;
    Ref<VulkanRenderPass>  m_render_pass;
    std::vector<VkFramebuffer> m_framebuffers;
    std::vector<VkCommandBuffer> m_command_buffers;
    u32                    m_queue_family = static_cast<u32>(-1);
    u32                    m_swapchain_buffering = 0;
    u32                    m_current_image_index = 0;
    const u32              m_min_image_count = 2;

    friend class VulkanShader;
    friend class VulkanRendererAPI;
    static VulkanContext *s_Instance;
};

}

#endif