// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "Origin/Renderer/GraphicsContext.h"

#include <vulkan.h>
#include <GLFW/glfw3.h>

namespace origin
{
    struct Swapchain
    {
        VkSwapchainKHR Handle;
        std::vector<VkImage> Images;
        std::vector<VkImageView> ImageViews;

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

        void CreateSwapchain();

    private:
        // Context
        VkInstance m_Instance;
        VkSurfaceKHR m_Surface;
        VkPhysicalDevice m_PhysicalDevice;
        VkDevice m_Device;
        VkQueue m_Queue;
        VkComponentMapping m_SwapchainComponentsMapping;
        uint32_t m_SwapchainBuffering;
        VkClearValue m_BackgroundColor;
        GLFWwindow *m_WindowHandle;
        uint32_t m_QueueFamilyIndex = 0;
        Swapchain m_Swapchain;
    };
}

#endif