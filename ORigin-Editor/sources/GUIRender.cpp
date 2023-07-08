#include "Editor.h"

#include "Origin\Audio\Audio.h"

namespace origin {

	void Editor::GUIRender()
	{
		if (guiAudioLibraryWindow)
		{
			ImGui::Begin("Audio Library", &guiAudioLibraryWindow);
			ImGui::Text("List: ");
			for (const auto& map : AudioEngine::GetMap())
				ImGui::Text("%s", map.first.c_str());

			if (ImGui::Button("Clear All"))
				AudioEngine::GetMap().clear();

			ImGui::End();
		}

		if (guiAudioCreationWindow)
		{
			ImGui::Begin("Audio Creation", &guiAudioCreationWindow);

			ImGui::End();
		}

		if (guiAnimationWindow)
		{
			ImGui::Begin("Animator", &guiAnimationWindow);
			if (Entity entity = m_SceneHierarchy.GetSelectedEntity())
			{
				ImGui::Text("This is %s Animator", entity.GetName().c_str());
				if (entity.HasComponent<AnimationComponent>())
				{
					auto& animComponent = entity.GetComponent<AnimationComponent>();
					auto& animation = animComponent.Animation;
					auto& state = animComponent.State;

					// Insert State name
					static std::string stateName = state.GetCurrentStateString();
					ImGui::Text("State Name: "); ImGui::SameLine();
					char buffer[256];
					strcpy_s(buffer, sizeof(buffer), stateName.c_str());
					if (ImGui::InputText("##stateName", buffer, sizeof(buffer)))
						stateName = std::string(buffer);

					ImGui::SameLine();
					if (ImGui::Button("+"))
					{
						if (!stateName.empty())
						{
							state.AddState(stateName);
							OGN_CORE_TRACE("Animation State {} added", stateName);
						}
						else
						{
							OGN_CORE_ERROR("Animation State failed to created");
						}
					}

					if (entity.HasComponent<SpriteRenderer2DComponent>() && state.HasState())
					{
						ImVec2 buttonSize = ImVec2(80.0f, 30.0f);
						ImGui::Button("Drop Texture", buttonSize);

						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
							{
								const wchar_t* path = (const wchar_t*)payload->Data;
								std::filesystem::path texturePath = Project::GetAssetFileSystemPath(path);

								if (texturePath.extension() == ".png" || texturePath.extension() == ".jpg")
								{
									auto texture = Texture2D::Create(texturePath.string());
									animation->AddFrame(texture, 0.23f);
								}
							}
						}
						ImGui::SameLine();
						if (ImGui::Button("Add Animation", buttonSize))
						{
							state.AddAnim(animation);
							OGN_CORE_TRACE("Animation added to {}", stateName);
						}

						// drop-down
						bool isSelected = false;
						auto& stateMap = state.GetAnimationState();
						auto& stateStorage = state.GetStateStorage();

						std::string currentState = state.GetCurrentStateString();

						if (ImGui::BeginCombo("ANIMATION STATE", currentState.c_str()))
						{
							for (int i = 0; i < stateStorage.size(); i++)
							{
								isSelected = currentState == stateStorage[i];
								if (ImGui::Selectable(stateStorage[i].c_str(), isSelected))
								{
									currentState = stateStorage[i];
									state.SetActiveState(stateStorage[i]);
								}
								if (isSelected) ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						} // !drow-down

						if (state.HasAnimation())
						{
							for (int i = 0; i < state.GetAnimation()->GetFrameTotal(); i++)
							{
								ImTextureID animTexture = (ImTextureID)state.GetAnimation()->GetCurrentSprite()->GetRendererID();
								ImGui::Image(animTexture, ImVec2(80.0f, 80.0f), ImVec2(0, 1), ImVec2(1, 0));
								ImGui::SameLine();
							}

							bool looping = state.IsLooping();
							if (ImGui::Checkbox("Loop", &looping)) state.SetLooping(looping);
							if (ImGui::Button("Delete State")) state.RemoveState(currentState);
						}
					}
				}
			}
			ImGui::End();
		}

		if (guiRenderStatusWindow)
		{
			ImGui::Begin("Render Status", &guiRenderStatusWindow);

			const char* CMSTypeString[] = { "PIVOT", "FREE MOVE" };
			const char* currentCMSTypeString = CMSTypeString[(int)m_EditorCamera.GetStyle()];
			ImGui::Text("Distance %.5f", m_EditorCamera.GetDistance());

			if (ImGui::BeginCombo("CAMERA STYLE", currentCMSTypeString)) {
				for (int i = 0; i < 2; i++) {
					const bool isSelected = currentCMSTypeString == CMSTypeString[i];
					if (ImGui::Selectable(CMSTypeString[i], isSelected)) {
						currentCMSTypeString = CMSTypeString[i];
						m_EditorCamera.SetStyle((CameraStyle)i);

					} if (isSelected) ImGui::SetItemDefaultFocus();
				} ImGui::EndCombo();
			}
			if (ImGui::SliderFloat("FOV", &m_CameraFov, 0.0f, 90.0f))
				m_EditorCamera.SetFov(m_CameraFov);

			ImGui::Checkbox("Visualize 2D Colliders", &m_VisualizeCollider);

			ImGui::Separator();
			ImGui::Text("Skybox");
			ImGui::Text("Blur"); ImGui::SameLine(); if (ImGui::DragFloat("##blur", &skyBlur, 0.0005f, 0.0f, 1.0f)) skybox->SetBlur(skyBlur);
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
