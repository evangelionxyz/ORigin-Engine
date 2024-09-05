// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "GuiLayer.h"
#include "Origin/Core/Application.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.cpp>

#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#ifdef OGN_PLATFORM_WINDOWS
#include <backends/imgui_impl_dx11.h>
#include "Platform/DX11/DX11Context.h"
#endif

#include "Platform/Vulkan/VulkanContext.h"

namespace origin
{
    static ImGui_ImplVulkanH_Window s_VulkanWindowData;

    static void FrameRender(ImGui_ImplVulkanH_Window *wd, ImDrawData *draw_data, VulkanContext *vk_context)
    {
        VkResult result;
        VkSemaphore image_acquired_semaphore  = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
        VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;

        result = vkAcquireNextImageKHR(vk_context->GetVkDevice(), wd->Swapchain, std::numeric_limits<uint64_t>::max(), image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            vk_context->SetRebuildSwapchain(true);
            return;
        }

        VkErrorCheck(result);

        ImGui_ImplVulkanH_Frame *fd = &wd->Frames[wd->FrameIndex];
        result = vkWaitForFences(vk_context->GetVkDevice(), 1, &fd->Fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
        VkErrorCheck(result);

        result = vkResetFences(vk_context->GetVkDevice(), 1, &fd->Fence);
        VkErrorCheck(result);

        result = vkResetCommandPool(vk_context->GetVkDevice(), fd->CommandPool, 0);
        VkErrorCheck(result);
        VkCommandBufferBeginInfo command_buffer_begin_info = {};
        command_buffer_begin_info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        command_buffer_begin_info.flags                   |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        result = vkBeginCommandBuffer(fd->CommandBuffer, &command_buffer_begin_info);
        VkErrorCheck(result);

        VkRenderPassBeginInfo render_pass_begin_info = {};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_begin_info.renderPass = wd->RenderPass;
        render_pass_begin_info.framebuffer = fd->Framebuffer;
        render_pass_begin_info.renderArea.extent.width = wd->Width;
        render_pass_begin_info.renderArea.extent.height = wd->Height;
        render_pass_begin_info.clearValueCount = 1;
        render_pass_begin_info.pClearValues = vk_context->GetClearValue();
        vkCmdBeginRenderPass(fd->CommandBuffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

        // submit command buffer
        vkCmdEndRenderPass(fd->CommandBuffer);
        VkPipelineStageFlags wait_stage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submit_info         = {};
        submit_info.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount   = 1;
        submit_info.pWaitSemaphores      = &image_acquired_semaphore;
        submit_info.pWaitDstStageMask    = &wait_stage;
        submit_info.commandBufferCount   = 1;
        submit_info.pCommandBuffers      = &fd->CommandBuffer;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores    = &render_complete_semaphore;

        result = vkEndCommandBuffer(fd->CommandBuffer);
        VkErrorCheck(result);

        result = vkQueueSubmit(vk_context->GetVkQueue(), 1, &submit_info, fd->Fence);
        VkErrorCheck(result);
    }

    static void FramePresent(ImGui_ImplVulkanH_Window *wd, VulkanContext *vk_context)
    {
        if (vk_context->IsRebuildSwapcahin())
        {
            return;
        }

        VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
        VkPresentInfoKHR info   = {};
        info.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores    = &render_complete_semaphore;
        info.swapchainCount     = 1;
        info.pSwapchains        = &wd->Swapchain;
        info.pImageIndices      = &wd->FrameIndex;

        VkResult result = vkQueuePresentKHR(vk_context->GetVkQueue(), &info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            vk_context->SetRebuildSwapchain(true);
            return;
        }

        VkErrorCheck(result);

        // now we can use the nwxt set of semaphore
        wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount;
    }

    static void SetupVulkanWindow(ImGui_ImplVulkanH_Window *wd, VulkanContext *vk_context, int width, int height)
    {
        VkBool32 res;
        wd->Surface = vk_context->GetVkSurface();
        vkGetPhysicalDeviceSurfaceSupportKHR(vk_context->GetVkPhysicalDevice(), vk_context->GetVkQueueFamily(), wd->Surface, &res);
        if (res != VK_TRUE)
        {
            OGN_CORE_ASSERT(false, "Error no WSI support on physical device");
            exit(-1);
        }

        // select surface format
        const VkFormat requestSurfaceImageFormat[] =
        {
            VK_FORMAT_B8G8R8A8_UNORM,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_FORMAT_B8G8R8_UNORM,
            VK_FORMAT_R8G8B8_UNORM
        };
        const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(vk_context->GetVkPhysicalDevice(), wd->Surface,
            requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

        // select present mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
        VkPresentModeKHR present_modes[] =
        {
            VK_PRESENT_MODE_MAILBOX_KHR,
            VL_PRESENT_MODE_IMMEDIATE_KHR,
            VK_PRESENT_MODE_FIFO_KHR
        };
#else
        VkPresentModeKHR present_modes[] =
        {
            VK_PRESENT_MODE_FIFO_KHR
        };
#endif
        wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(vk_context->GetVkPhysicalDevice(), wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
        ImGui_ImplVulkanH_CreateOrResizeWindow(vk_context->GetVkInstance(), vk_context->GetVkPhysicalDevice(),
            vk_context->GetVkDevice(), wd, vk_context->GetVkQueueFamily(), vk_context->GetVkAllocator(), width, height,
            vk_context->GetVkMinImageCount());

        OGN_CORE_INFO("[Vulkan] Vulkan window was created");
    }

    GuiLayer::GuiLayer(const std::shared_ptr<Window> &window)
        : Layer("Gui Layer"), m_WindowContext(window)
    {
    }

    void GuiLayer::Init()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

#pragma region theme_init
        ImGuiStyle &style = ImGui::GetStyle();
        ImVec4 *colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.31f, 0.31f, 0.31f, 0.75f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.91f, 0.91f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
        colors[ImGuiCol_TabSelected] = ImVec4(0.48f, 0.48f, 0.48f, 1.00f);
        colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
        colors[ImGuiCol_TabDimmed] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
        colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.78f, 0.52f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.67f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.45f, 0.45f, 0.45f, 0.50f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.67f, 0.00f, 1.00f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.90f, 0.90f, 0.90f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.45f);

        style.WindowPadding = ImVec2{0.0f, 0.0f};
        style.FramePadding = ImVec2{6.0f, 3.0f};
        style.CellPadding = ImVec2{7.0f, 3.0f};
        style.ItemSpacing = ImVec2{4.0f, 3.0f};
        style.ItemInnerSpacing = ImVec2{6.0f, 3.0f};
        style.TouchExtraPadding = ImVec2{0.0f, 0.0f};
        style.IndentSpacing = 8;
        style.ScrollbarSize = 16.0f;
        style.GrabMinSize = 13;

        style.WindowBorderSize = 0;
        style.ChildBorderSize = 1;
        style.PopupBorderSize = 1;
        style.FrameBorderSize = 0;
        style.TabBorderSize = 0;

        style.WindowRounding = 0;
        style.ChildRounding = 0;
        style.FrameRounding = 0;
        style.PopupRounding = 0;
        style.ScrollbarRounding = 0;

        style.WindowMenuButtonPosition = ImGuiDir_Right;
        style.ColorButtonPosition = ImGuiDir_Right;

        style.AntiAliasedFill = true;
        style.AntiAliasedLines = true;
        style.AntiAliasedLinesUseTex = true;

        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigViewportsNoDecoration = false;

        float fontSize = 16.0f;
        io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/segoeui.ttf", fontSize);
#pragma endregion

        switch (RendererAPI::GetAPI())
        {
#ifdef OGN_PLATFORM_WINDOWS
        case RendererAPI::API::DX11:
        {
            DX11Context *dx_context = DX11Context::GetInstance();
            ImGui_ImplGlfw_InitForOther((GLFWwindow *)m_WindowContext->GetNativeWindow(), true);
            ImGui_ImplDX11_Init(dx_context->Device, dx_context->DeviceContext);
            break;
        }
#endif
        case RendererAPI::API::OpenGL:
        {
            ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)m_WindowContext->GetNativeWindow(), true);
            ImGui_ImplOpenGL3_Init("#version 450");
            break;
        }
        case RendererAPI::API::Vulkan:
        {
            // Creating vulkan window
            VulkanContext *vk_context = VulkanContext::GetInstance();
            int width, height;
            glfwGetFramebufferSize((GLFWwindow *)m_WindowContext->GetNativeWindow(), &width, &height);
            ImGui_ImplVulkanH_Window *wd = &s_VulkanWindowData;
            SetupVulkanWindow(wd, vk_context, width, height);

            ImGui_ImplGlfw_InitForVulkan((GLFWwindow *)m_WindowContext->GetNativeWindow(), true);
            ImGui_ImplVulkan_InitInfo init_info = {};
            init_info.Instance        = vk_context->GetVkInstance();
            init_info.PhysicalDevice  = vk_context->GetVkPhysicalDevice();
            init_info.Device          = vk_context->GetVkDevice();
            init_info.QueueFamily     = vk_context->GetVkQueueFamily();
            init_info.Queue           = vk_context->GetVkQueue();
            init_info.PipelineCache   = vk_context->GetVkPipelineCache();
            init_info.DescriptorPool  = vk_context->GetVkDescriptorPool();
            init_info.Subpass         = 0;
            init_info.MinImageCount   = vk_context->GetVkMinImageCount();
            init_info.ImageCount      = wd->ImageCount;
            init_info.MSAASamples     = VK_SAMPLE_COUNT_1_BIT;
            init_info.Allocator       = vk_context->GetVkAllocator();
            init_info.RenderPass      = wd->RenderPass;
            init_info.CheckVkResultFn = VkErrorCheck;

            ImGui_ImplVulkan_Init(&init_info);
        }
        break;
        }
    }

    void GuiLayer::OnDetach()
    {
        switch (RendererAPI::GetAPI())
        {
#ifdef OGN_PLATFORM_WINDOWS
        case RendererAPI::API::DX11:
        {
            ImGui_ImplDX11_Shutdown();
            break;
        }
#endif
        case RendererAPI::API::OpenGL:
        {
            ImGui_ImplOpenGL3_Shutdown();
            break;
        }
        case RendererAPI::API::Vulkan:
        {
            VulkanContext *vk_context = VulkanContext::GetInstance();
            VkResult result = vkDeviceWaitIdle(vk_context->GetVkDevice());
            VkErrorCheck(result);
            ImGui_ImplVulkanH_DestroyWindow(vk_context->GetVkInstance(), 
                vk_context->GetVkDevice(), &s_VulkanWindowData, vk_context->GetVkAllocator());
            ImGui_ImplVulkan_Shutdown();
            
            break;
        }
        }
        
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void GuiLayer::OnAttach()
    {
        Init();
    }

    GuiLayer::~GuiLayer()
    {
        OnDetach();
    }

    void GuiLayer::Begin()
    {
        ImGui_ImplGlfw_NewFrame();

        switch (RendererAPI::GetAPI())
        {
#ifdef OGN_PLATFORM_WINDOWS
        case RendererAPI::API::DX11:
        {
            ImGui_ImplDX11_NewFrame();
            break;
        }
#endif
        case RendererAPI::API::OpenGL:
        {
            ImGui_ImplOpenGL3_NewFrame();
            break;
        }
        case RendererAPI::API::Vulkan:
        {
            glfwPollEvents();
            VulkanContext *vk_context = VulkanContext::GetInstance();
            if(vk_context->IsRebuildSwapcahin())
            {
                int width, height;
                glfwGetFramebufferSize((GLFWwindow *)m_WindowContext->GetNativeWindow(), &width, &height);
                if(width > 0 && height > 0)
                {
                    ImGui_ImplVulkan_SetMinImageCount(vk_context->GetVkMinImageCount());
                    ImGui_ImplVulkanH_CreateOrResizeWindow(vk_context->GetVkInstance(), vk_context->GetVkPhysicalDevice(),
                        vk_context->GetVkDevice(), &s_VulkanWindowData,
                        vk_context->GetVkQueueFamily(), vk_context->GetVkAllocator(),
                        width, height, vk_context->GetVkMinImageCount());
                    s_VulkanWindowData.FrameIndex = 0;
                    vk_context->SetRebuildSwapchain(false);
                    OGN_CORE_INFO("[Vulkan] Rebuild swapchain!");
                }
            }
            ImGui_ImplVulkan_NewFrame();
            break;
        }
        }

        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void GuiLayer::End()
    {
        ImGuiIO &io = ImGui::GetIO();

        Application &app = Application::Instance();
        int width, height;
        glfwGetFramebufferSize((GLFWwindow *)m_WindowContext->GetNativeWindow(), &width, &height);
        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));

        // ====================
        // Rendering 
        ImGui::Render();
        switch (RendererAPI::GetAPI())
        {
#ifdef OGN_PLATFORM_WINDOWS
        case RendererAPI::API::DX11:
        {
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent((GLFWwindow *)m_WindowContext->GetNativeWindow());
            }

            break;
        }
#endif
        case RendererAPI::API::OpenGL:
         {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent((GLFWwindow *)m_WindowContext->GetNativeWindow());
            }
            break;
        }
        case RendererAPI::API::Vulkan:
        {
            ImGui_ImplVulkanH_Window *wd = &s_VulkanWindowData;
            ImDrawData *main_draw_data = ImGui::GetDrawData();
            const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);

            VulkanContext *vk_context = VulkanContext::GetInstance();
            if(!main_is_minimized)
                FrameRender(wd, main_draw_data, vk_context);

            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
            
            if (!main_is_minimized)
                FramePresent(wd, vk_context);
            break;
        }
        }
    }

    uint32_t GuiLayer::GetActiveWidgetID()
    {
        return GImGui->ActiveId;
    }

    void GuiLayer::OnEvent(Event &e)
    {
        if (m_BlockEvents)
        {
            ImGuiIO &io = ImGui::GetIO();
            e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
            e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
        }
    }
}
