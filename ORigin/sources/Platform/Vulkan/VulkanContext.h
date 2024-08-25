// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "Origin/Renderer/GraphicsContext.h"

#ifdef OGN_PLATFORM_WINDOWS
    #define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

namespace origin
{

    static void VkErrorCheck(VkResult err)
    {
        if (err == 0)
            return;
        
        OGN_CORE_ERROR("[Vulkan] Error: VkResult {}", err);
        if (err < 0)
        {
            abort();
        }
    }
    struct Swapchain
    {
        VkSwapchainKHR Handle;
        std::vector<VkImage> Images;
        std::vector<VkImageView> Views;

        VkFormat ImageFormat;
        VkExtent2D Extent;
    };

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
        uint32_t GetVkQueueFamily()             { return m_QueueFamily; }
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
        VkAllocationCallbacks *m_Allocator      = NULL;
        VkInstance             m_Instance       = VK_NULL_HANDLE;
        VkSurfaceKHR           m_Surface        = VK_NULL_HANDLE;
        VkPhysicalDevice       m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice               m_Device         = VK_NULL_HANDLE;
        VkDescriptorPool       m_DescriptorPool = VK_NULL_HANDLE;
        VkQueue                m_Queue          = VK_NULL_HANDLE;
        uint32_t               m_QueueFamily    = (uint32_t)-1;
        VkPipelineCache        m_PipelineCache  = VK_NULL_HANDLE;
        VkComponentMapping     m_SwapchainComponentsMapping;
        uint32_t               m_SwapchainBuffering;
        VkClearValue           m_ClearValue;
        GLFWwindow            *m_WindowHandle;
        Swapchain              m_Swapchain;
        bool                   m_RebuildSwapchain = false;
        int                    m_MinImageCount = 2;
        static inline uint32_t s_CurrentFrameIndex = 0;

        friend class VulkanShader;
        friend class VulkanRendererAPI;
        static VulkanContext *s_Instance;
    };
}

#endif