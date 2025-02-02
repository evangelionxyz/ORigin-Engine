// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

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

    void SetRebuildSwapchain(bool enable) { m_RebuildSwapchain = enable; }
    bool IsRebuildSwapcahin() { return m_RebuildSwapchain; }
    u32 GetVkQueueFamily() { return m_QueueFamily; }
    const u32 GetMinImageCount() const { return m_MinImageCount; }
    VulkanSwapchain *GetSwapchain() { return &m_Swapchain; }
    VkDevice GetVkDevice() { return m_LogicalDevice; }
    VkInstance GetVkInstance() { return m_Instance; }
    VkPhysicalDevice GetVkPhysicalDevice() { return m_PhysicalDevice.GetSelectedDevice().Device; }
    VkQueue GetVkQueue() { return m_Queue.GetVkQueue(); }
    VulkanQueue *GetQueue() { return &m_Queue; }
    VkSurfaceKHR GetVkSurface() { return m_Surface; }
    VkDescriptorPool GetVkDescriptorPool() { return m_DescriptorPool; }
    VkPipelineCache GetVkPipelineCache() { return m_PipelineCache; }
    VkRenderPass GetVkRenderPass() const 
    { 
        return m_render_pass->GetRenderPass(); 
    }

    VkClearValue *GetClearValue() { return &m_ClearValue; }
    VkFramebuffer GetFramebuffer(u32 index) { return m_Framebuffers[index]; }
    VkCommandPool GetVkCommandPool() { return m_CommandPool; }
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

    void Present();

    void BeginFrame();
    void EndFrame();
    void RecordCommandBuffer(VkCommandBuffer command_buffer, u32 image_index);

    using CommandCallback = std::function<void(VkCommandBuffer, VkFramebuffer, u32)>;

    void Submit(CommandCallback callback);

private:
    std::vector<CommandCallback> m_CommandCallbacks;
    u32 m_current_image_index = 0;

    // Context
    VkInstance             m_Instance = VK_NULL_HANDLE;
    VkSurfaceKHR           m_Surface = VK_NULL_HANDLE;
    VkDevice               m_LogicalDevice = VK_NULL_HANDLE;
    VkCommandPool          m_CommandPool = VK_NULL_HANDLE;
    VkDescriptorPool       m_DescriptorPool = VK_NULL_HANDLE;
    VkPipelineLayout       m_PipelineLayout = VK_NULL_HANDLE;
    VkPipelineCache        m_PipelineCache = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    GLFWwindow            *m_WindowHandle = nullptr;
    VkComponentMapping     m_SwapchainComponentsMapping{};
    VulkanPhysicalDevice   m_PhysicalDevice;
    VulkanQueue            m_Queue;
    VkClearValue           m_ClearValue{};
    VulkanSwapchain        m_Swapchain;
    u32                    m_QueueFamily = static_cast<u32>(-1);
    u32                    m_SwapchainBuffering = 0;
    const u32              m_MinImageCount = 2;
    bool                   m_RebuildSwapchain = false;
    Ref<VulkanRenderPass>  m_render_pass;

    std::vector<VkFramebuffer> m_Framebuffers;
    std::vector<VkCommandBuffer> m_CommandBuffers;

    friend class VulkanShader;
    friend class VulkanRendererAPI;
    static VulkanContext *s_Instance;
};

}

#endif