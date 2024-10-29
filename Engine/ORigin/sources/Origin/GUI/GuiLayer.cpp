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
            break;
        }
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
