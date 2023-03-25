// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "GuiLayer.h"
#include "Origin\Core\OriginCore.h"
#include "Origin\Core\Application.h"

#include <imgui.h>
#include <backends\imgui_impl_glfw.h>
#include <backends\imgui_impl_opengl3.h>

#include <ImGuizmo.h>

namespace Origin {

	void GuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::StyleColorsDark();

		float fontSize = 16.0f;
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/segoeui.ttf", fontSize);

		style.ScrollbarSize = 10.0f;
		style.GrabMinSize = 12.0f;

		style.TabBorderSize = 0;
		style.WindowBorderSize = 0;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigViewportsNoDecoration = true;

		Application& app = Application::Get();

		ImGui_ImplGlfw_InitForOpenGL(app.GetWindow().GetMainWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	GuiLayer::~GuiLayer()
	{
		OnDetach();
	}

	void GuiLayer::OnUpdate(Timestep ts)
	{
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
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());
		
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

	void GuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
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
