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

#include "Platform/Vulkan/VulkanContext.hpp"

namespace origin
{
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
        //io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/segoeui.ttf", fontSize);
#pragma endregion

        switch (RendererAPI::GetAPI())
        {
#ifdef OGN_PLATFORM_WINDOWS
        case RendererAPI::API::DX11:
        {
            DX11Context *dx_context = DX11Context::GetInstance();
            ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow *>(m_WindowContext->GetNativeWindow()), true);
            ImGui_ImplDX11_Init(dx_context->Device, dx_context->DeviceContext);
            break;
        }
#endif
        case RendererAPI::API::OpenGL:
        {
            ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(m_WindowContext->GetNativeWindow()), true);
            ImGui_ImplOpenGL3_Init("#version 450");
            break;
        }
        case RendererAPI::API::Vulkan:
        {
            constexpr bool install_callbacks = true;
            ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow *>(m_WindowContext->GetNativeWindow()), install_callbacks);

            auto vk_context = GraphicsContext::GetContext<VulkanContext>();
            ImGui_ImplVulkan_InitInfo init_info = {};

            init_info.Instance = vk_context->GetVkInstance();
            init_info.PhysicalDevice = vk_context->GetVkPhysicalDevice();
            init_info.Device = vk_context->GetVkDevice();
            init_info.QueueFamily = vk_context->GetVkQueueFamily();
            init_info.Queue = vk_context->GetVkQueue();
            init_info.PipelineCache = vk_context->GetVkPipelineCache();
            init_info.DescriptorPool = vk_context->GetVkDescriptorPool();
            init_info.RenderPass = vk_context->GetVkRenderPass();
            init_info.Subpass = 0;
            init_info.MinImageCount = vk_context->GetVkMinImageCount();
            init_info.ImageCount = vk_context->GetSwapchain()->GetVkImageCount();
            init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
            init_info.Allocator = vk_context->GetVkAllocator();
            init_info.CheckVkResultFn = VK_NULL_HANDLE;

            ImGui_ImplVulkan_Init(&init_info);
            break;
        }
        }
    }

    void GuiLayer::OnDetach()
    {
        ImGui_ImplGlfw_Shutdown();

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
            ImGui_ImplVulkan_Shutdown();
            break;
        }
        }
        
        ImGui::DestroyContext();
    }

    void GuiLayer::OnAttach()
    {
        Init();
    }

    void GuiLayer::Begin()
    {

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
            ImGui_ImplVulkan_NewFrame();
            break;
        }
        }

        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void GuiLayer::End()
    {
        ImGuiIO &io = ImGui::GetIO();

        Application &app = Application::GetInstance();
        int width, height;
        glfwGetFramebufferSize(static_cast<GLFWwindow *>(m_WindowContext->GetNativeWindow()), &width, &height);
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
                glfwMakeContextCurrent(static_cast<GLFWwindow *>(m_WindowContext->GetNativeWindow()));
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
                glfwMakeContextCurrent(static_cast<GLFWwindow *>(m_WindowContext->GetNativeWindow()));
            }
            break;
        }
        case RendererAPI::API::Vulkan:
        {
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }
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
