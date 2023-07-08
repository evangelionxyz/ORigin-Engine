// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "..\Editor.h"
#include "Origin\Audio\Audio.h"
#include "Origin\Scripting\ScriptEngine.h"

namespace origin
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
				if (ImGui::MenuItem("New Project")) NewProject();
				if (ImGui::MenuItem("Open Project")) OpenProject();
				ImGui::Separator();
				if (ImGui::MenuItem("New Scene", "Ctrl+N")) NewScene();
				if (ImGui::MenuItem("Open Scene", "Ctrl+O"))  OpenScene();
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))  SaveScene();
				if (ImGui::MenuItem("Save Scene As", "Ctrl+Shift+S"))  SaveSceneAs();
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

			if (ImGui::BeginMenu("Object"))
			{
				ImGui::MenuItem("Audio Library", nullptr, &guiAudioLibraryWindow);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				ImGui::MenuItem("Style Editor", nullptr, &guiMenuStyle);
				ImGui::MenuItem("Render Status", nullptr, &guiRenderStatusWindow);
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
	}
}