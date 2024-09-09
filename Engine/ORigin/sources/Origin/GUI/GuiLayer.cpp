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
