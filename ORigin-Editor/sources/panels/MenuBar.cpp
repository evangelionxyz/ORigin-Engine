// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "..\Editor.h"
#include "Origin\Scripting\ScriptEngine.h"

namespace Origin
{
	void Editor::MenuBar()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		auto& window = Application::Get();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		if (ImGui::BeginMenuBar())
		{
			ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N")) NewScene();
				if (ImGui::MenuItem("Open", "Ctrl+O"))  OpenScene();
				if (ImGui::MenuItem("Save", "Ctrl+S"))  SaveScene();
				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))  SaveSceneAs();
				if (ImGui::MenuItem("Exit", "Alt+F4")) window.Close();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Script"))
			{
				if (ImGui::MenuItem("Reload"))
				{
					if(m_SceneState == SceneState::Play || m_SceneState == SceneState::Simulate)
						OnSceneStop();

					ScriptEngine::ReloadAssembly();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				ImGui::MenuItem("Style Editor", nullptr, &guiMenuStyle);
				ImGui::MenuItem("Render Status", nullptr, &guiRenderStatus);
				ImGui::MenuItem("Debug Info", nullptr, &guiDebugInfo);
				ImGui::MenuItem("Demo Window", nullptr, &guiImGuiDemoWindow);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Fullscreen", "F11", &guiMenuFullscreen))
					window.GetWindow().SetFullscreen(guiMenuFullscreen);
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}

		if (guiRenderStatus)
		{
			ImGui::Begin("Render Status", &guiRenderStatus);

			ImGui::Checkbox("Visualize 2D Colliders", &m_VisualizeCollider);

			ImGui::Separator();
			ImGui::Text("Skybox");
			ImGui::Text("Blur"); ImGui::SameLine(); if(ImGui::DragFloat("##blur", &skyBlur, 0.0005f, 0.0f, 1.0f)) skybox->SetBlur(skyBlur);
			ImGui::Checkbox("Enable", &enableSkybox);
			ImGui::Separator();

			ImGui::Text("Grid");
			ImGui::Text("Size "); ImGui::SameLine(0.0f, 1.5f); ImGui::DragInt("##grid_size", &m_GridSize);
			ImGui::Text("Color"); ImGui::SameLine(0.0f, 1.5f); ImGui::ColorEdit4("##grid_color", glm::value_ptr(m_GridColor));
			m_ActiveScene->SetGrid(m_GridSize, m_GridColor);

			ImGui::Separator();

			const auto Stats2D = Renderer2D::GetStats();
			ImGui::Text("2D Stats");
			ImGui::Text("Draw Calls: %d", Stats2D.DrawCalls);
			ImGui::Text("Quads %d", Stats2D.QuadCount);
			ImGui::SameLine(); ImGui::Text("Circles %d", Stats2D.CircleCount);
			ImGui::SameLine(); ImGui::Text("Lines %d", Stats2D.LineCount);
			ImGui::Text("Vertices: %d", Stats2D.GetTotalVertexCount());
			ImGui::Text("Indices: %d", Stats2D.GetTotalIndexCount());
			ImGui::Separator();

			const auto Stats3D = Renderer3D::GetStats();
			ImGui::Text("3D Stats");
			ImGui::Text("Draw Calls: %d", Stats3D.DrawCalls);
			ImGui::Text("Cube %d", Stats3D.CubeCount);
			ImGui::Text("Vertices: %d", Stats3D.GetTotalVertexCount());
			ImGui::Text("Indices: %d", Stats3D.GetTotalIndexCount());
			ImGui::Separator();

			if (ImGui::Checkbox("Line Mode", &drawLineMode)) RenderCommand::DrawLineMode(drawLineMode);
			ImGui::SameLine(0.0f, 1.5f); ImGui::ColorEdit4("Background Color", glm::value_ptr(clearColor));

			const char* RTTypeString[] = { "Normal" };
			const char* currentRTTypeString = RTTypeString[m_RenderTarget];

			if (ImGui::BeginCombo("Render Target", currentRTTypeString)) {
				for (int i = 0; i < 1; i++) {
					const bool isSelected = currentRTTypeString == RTTypeString[i];
					if (ImGui::Selectable(RTTypeString[i], isSelected)) {
						currentRTTypeString = RTTypeString[i];
						m_RenderTarget = i;
					} if (isSelected) ImGui::SetItemDefaultFocus();
				} ImGui::EndCombo();
			}

			ImGui::End();
		}

		if (guiMenuStyle)
		{
			ImGui::Begin("Style Editor", &guiMenuStyle);
			ImGui::ShowStyleEditor();
			ImGui::End();
		}

		if (guiDebugInfo)
		{
			ImGui::Begin("Debug Info", &guiDebugInfo);

			ImGui::Text("Time (%.2f s)", m_Time);
			if (ImGui::Button("Reset Time")) { m_Time = 0.0f; }
			ImGui::Text("OpenGL Version : (%s)", glGetString(GL_VERSION));
			ImGui::Text("ImGui version : (%s)", IMGUI_VERSION);
			ImGui::Text("ImGuizmo Hovered (%d)", ImGuizmo::IsOver());
			ImGui::Text("Viewport Hovered (%d)", m_SceneViewportHovered);
			ImGui::Text("Hierarchy Menu Activity (%d)", m_SceneHierarchy.GetHierarchyMenuActive());
			ImGui::End();
		}

		if (guiImGuiDemoWindow)
		{
			ImGui::ShowDemoWindow(&guiImGuiDemoWindow);
		}
	}
}