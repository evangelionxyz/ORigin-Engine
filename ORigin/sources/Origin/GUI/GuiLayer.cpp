// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "GuiLayer.h"
#include "Origin\Core\Application.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_dx11.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include "Platform/DX11/DX11Context.h"

namespace origin {

	GuiLayer::GuiLayer(const std::shared_ptr<Window> &window)
		: Layer("Gui Layer"), m_WindowContext(window)
	{
	}

	void GuiLayer::Init()
	{
		OGN_PROFILER_UI();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

#pragma region theme_init

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.32f, 0.32f, 0.32f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.26f, 0.26f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 0.54f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.85f, 0.53f, 0.00f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.21f, 0.21f, 0.21f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.33f, 0.28f, 0.25f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.38f, 0.38f, 0.38f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.98f, 0.94f, 0.26f, 0.67f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.98f, 0.94f, 0.26f, 0.67f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.98f, 0.94f, 0.26f, 0.67f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.98f, 0.94f, 0.26f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.98f, 0.94f, 0.26f, 0.67f);
		colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.16f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.20f, 0.20f, 0.20f, 0.86f);
		colors[ImGuiCol_TabActive] = ImVec4(0.39f, 0.39f, 0.39f, 0.86f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.20f, 0.86f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.51f, 0.51f, 0.51f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2{ 0.0f, 0.0f };
		style.FramePadding = ImVec2{ 7.0f, 3.0f };
		style.CellPadding = ImVec2{ 0.0f, 3.0f };
		style.ItemSpacing = ImVec2{ 4.0f, 3.0f };
		style.ItemInnerSpacing = ImVec2{ 7.0f, 3.0f };
		style.TouchExtraPadding = ImVec2{ 0.0f, 0.0f };
		style.IndentSpacing = 20;
		style.ScrollbarSize = 14;
		style.GrabMinSize = 14;

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

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigViewportsNoDecoration = false;

		float fontSize = 16.0f;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/segoeui.ttf", fontSize);
#pragma endregion

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::DX11:
			{
				ImGui_ImplGlfw_InitForOther((GLFWwindow *)m_WindowContext->GetNativeWindow(), true);
				ImGui_ImplDX11_Init(DX11Context::Get()->Device, DX11Context::Get()->DeviceContext);
			}
			break;
		case RendererAPI::API::OpenGL:
			{
				ImGui_ImplGlfw_InitForOpenGL((GLFWwindow *)m_WindowContext->GetNativeWindow(), true);
				ImGui_ImplOpenGL3_Init("#version 450");
			}
			break;
		}
	}

	void GuiLayer::Detach()
	{
		OGN_PROFILER_UI();

		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::DX11: ImGui_ImplDX11_Shutdown(); break;
		case RendererAPI::API::OpenGL: ImGui_ImplOpenGL3_Shutdown(); break;
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

	void GuiLayer::SetDisplaySize(float width, float height)
	{
		m_Width = width;
		m_Height = height;
	}

	void GuiLayer::Begin()
	{
		OGN_PROFILER_UI();

		ImGui_ImplGlfw_NewFrame();
		
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::DX11: ImGui_ImplDX11_NewFrame(); break;
		case RendererAPI::API::OpenGL: ImGui_ImplOpenGL3_NewFrame(); break;
		}
		
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void GuiLayer::End() const
	{
		OGN_PROFILER_UI();

		ImGuiIO& io = ImGui::GetIO();

		Application& app = Application::Get();
		io.DisplaySize = ImVec2(m_Width, m_Height);
		
		ImGui::Render();

		switch(RendererAPI::GetAPI())
		{
		case RendererAPI::API::DX11: ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); break;
		case RendererAPI::API::OpenGL: ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); break;
		}

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_context_current = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_context_current);
		}
	}

	uint32_t GuiLayer::GetActiveWidgetID()
	{
		return GImGui->ActiveId;
	}

	void GuiLayer::OnDetach()
	{
		Detach();
	}

	void GuiLayer::OnEvent(Event& e)
	{
		OGN_PROFILER_UI();

		if(m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}
}
