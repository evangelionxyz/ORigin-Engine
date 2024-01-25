// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "GuiLayer.h"
#include "Origin\Core\Application.h"

#include <imgui.h>
#include <backends\imgui_impl_glfw.h>
#include <backends\imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <ImGuizmo.h>
#include <ImSequencer.h>

namespace origin {

	GuiLayer::GuiLayer()
		: Layer("Gui Layer"), m_WindowContext(nullptr)
	{
	}

	void GuiLayer::SetContext(GLFWwindow* window)
	{
		m_WindowContext = window;
	}

	void GuiLayer::Init()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
#if 0
		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.01f, 0.00f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.03f, 0.01f, 0.00f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.03f, 0.01f, 0.00f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.67f, 0.67f, 0.67f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.03f, 0.01f, 0.00f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.03f, 0.01f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.02f, 0.01f, 0.00f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.87f, 0.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.40f, 0.48f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.91f, 0.54f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.57f, 0.57f, 0.57f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.31f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.78f, 0.78f, 0.78f, 0.31f);
		colors[ImGuiCol_Separator] = ImVec4(0.36f, 0.36f, 0.36f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.50f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.36f, 0.36f, 0.36f, 0.50f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.22f, 0.22f, 0.22f, 0.31f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.50f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.22f, 0.22f, 0.22f, 0.31f);
		colors[ImGuiCol_Tab] = ImVec4(0.22f, 0.22f, 0.22f, 0.31f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.50f);
		colors[ImGuiCol_TabActive] = ImVec4(0.74f, 0.74f, 0.74f, 0.31f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.14f, 0.14f, 0.14f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.93f, 0.93f, 0.93f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.08f, 0.04f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

#endif
		ImGuiStyle& style = ImGui::GetStyle();
		style.GrabMinSize = 12.0f;
		style.TabBorderSize = 0;
		style.ScrollbarSize = 10.0f;
		style.GrabRounding = 2.0f;
		style.PopupRounding = 2.0f;
		style.IndentSpacing = 12.0f;
		style.FrameRounding = 2.0f;
		style.ChildRounding = 3.0f;
		style.WindowRounding = 3.0f;
		style.WindowBorderSize = 0;
		style.ScrollbarRounding = 2.0f;

		style.AntiAliasedFill = true;
		style.AntiAliasedLines = true;
		style.AntiAliasedLinesUseTex = true;

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigViewportsNoDecoration = true;

		float fontSize = 16.0f;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/segoeui.ttf", fontSize);

		ImGui_ImplGlfw_InitForOpenGL(m_WindowContext, true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	void GuiLayer::Detach()
	{
		ImGui_ImplOpenGL3_Shutdown();
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
		m_DisplaySize = ImVec2(width, height);
	}

	void GuiLayer::Begin()
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void GuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();

		Application& app = Application::Get();
		io.DisplaySize = ImVec2(m_DisplaySize);
		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
		if(m_BlockEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}
}
