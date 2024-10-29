// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "Origin/Renderer/GraphicsContext.h"
#include "VulkanSwapchain.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace origin
{
    class VulkanContext : public GraphicsContext
    {
    public:
        VulkanContext();
        void Init(Window *window) override;
        void Shutdown() override;

        void CreateVkInstance();
        void CreateVkPhysicalDevice();
        void CreateVkSurface();
        void CreateVkDevice();
        void CreateVkSwapchain();
        void CreateVkImageViews();
        void CreateVkDescriptorPool();
        void CreateGraphicsPipeline();

        static VulkanContext *GetInstance();

        void SetRebuildSwapchain(bool enable)   { m_RebuildSwapchain = enable; }
        bool IsRebuildSwapcahin()               { return m_RebuildSwapchain; }
        int GetVkMinImageCount()                { return m_MinImageCount; }
        u32 GetVkQueueFamily()                  { return m_QueueFamily; }
        VkAllocationCallbacks *GetVkAllocator() { return m_Allocator;}
        VkDevice GetVkDevice()                  { return m_Device; }
        VkInstance GetVkInstance()              { return m_Instance; }
        VkPhysicalDevice GetVkPhysicalDevice()  { return m_PhysicalDevice; }
        VkQueue GetVkQueue()                    { return m_Queue; }
        VkSurfaceKHR GetVkSurface()             { return m_Surface; }
        VkDescriptorPool GetVkDescriptorPool()  { return m_DescriptorPool; }
        VkPipelineCache GetVkPipelineCache()    { return m_PipelineCache; }
        VkClearValue *GetClearValue()           { return &m_ClearValue; }

    private:
        // Context
        VkAllocationCallbacks *m_Allocator      = VK_NULL_HANDLE;
        VkInstance             m_Instance       = VK_NULL_HANDLE;
        VkSurfaceKHR           m_Surface        = VK_NULL_HANDLE;
        VkPhysicalDevice       m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice               m_Device         = VK_NULL_HANDLE;
        VkDescriptorPool       m_DescriptorPool = VK_NULL_HANDLE;
        VkQueue                m_Queue          = VK_NULL_HANDLE;
        u32                    m_QueueFamily    = static_cast<u32>(-1);
        VkPipelineCache        m_PipelineCache  = VK_NULL_HANDLE;
        VkComponentMapping     m_SwapchainComponentsMapping{};
        u32                    m_SwapchainBuffering = 0;
        VkClearValue           m_ClearValue{};
        GLFWwindow            *m_WindowHandle     = nullptr;
        VulkanSwapchain        m_Swapchain;
        bool                   m_RebuildSwapchain = false;
        int                    m_MinImageCount = 2;
        static inline u32 s_CurrentFrameIndex = 0;

        friend class VulkanShader;
        friend class VulkanRendererAPI;
        static VulkanContext *s_Instance;
    };
}

#endif