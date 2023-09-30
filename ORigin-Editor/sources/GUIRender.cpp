#include "Editor.h"

#include "Origin\Audio\Audio.h"
#include "Origin\Utils\Utils.h"

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
				if (entity.HasComponent<AnimationComponent>())
				{
					ImGui::Text("%s Animator", entity.GetTag().c_str());

					auto& ac = entity.GetComponent<AnimationComponent>();
					auto& state = ac.State;

					// Insert State name
					static std::string stateName;

					ImGui::Text("State Name: "); ImGui::SameLine();

					char buffer[256];
					strcpy_s(buffer, sizeof(buffer), stateName.c_str());

					if (ImGui::InputText("##stateName", buffer, sizeof(buffer)))
						stateName = std::string(buffer);

					ImGui::SameLine();
					if (ImGui::Button("+", ImVec2(30.0f, 20.0f)))
					{
						if (!stateName.empty())
						{
							state.AddState(stateName);
							stateName.clear();
						}
					}

					if (entity.HasComponent<SpriteRenderer2DComponent>() && state.HasState())
					{
						ImGui::Text("Animation State");

						ImGui::SameLine();

						// drop-down
						auto& stateStorage = state.GetStateStorage();
						std::string currentState = state.GetCurrentState();

						if (ImGui::BeginCombo("##AnimationState", currentState.c_str()))
						{
							bool isSelected = false;
							for (const auto& st : stateStorage)
							{
								isSelected = currentState == st;
								if (ImGui::Selectable(st.c_str(), isSelected))
								{
									currentState = st;
									state.SetActiveState(st);
								}
								if (isSelected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						} // !drop-down
						
						static Animation animation;

						// Drag and Drop
						ImGui::Button("Drop Texture", ImVec2(80.0f, 30.0f));
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
							{
								const auto* path = static_cast<const wchar_t*>(payload->Data);
								const std::filesystem::path texturePath = Project::GetAssetFileSystemPath(path);

								if (texturePath.extension() == ".png" || texturePath.extension() == ".jpg")
								{
									const std::shared_ptr<Texture2D> texture = Texture2D::Create(texturePath.string());
									animation.AddFrame(texture, 0.23f);
								}
							}
						}

						ImGui::SameLine();
						if (ImGui::Button("Add Animation", { 90.0f, 30.0f }) && animation.HasFrame())
						{
							state.AddAnimation(animation);
							OGN_CORE_TRACE("Animation added to {}", stateName);

							animation.Delete();
						}

						ImGui::SameLine();
						if (ImGui::Button("Set As Default State", ImVec2(80.0f, 30.f)))
							state.SetDefaultState(state.GetCurrentState());

						// Display what texture has been dropped
						const float padding = 10.0f;
						const float imageSize = 42.0f;
						const float cellSize = imageSize + padding;

						const float panelWidth = ImGui::GetContentRegionAvail().x;
						int columnCount = static_cast<int>(panelWidth / cellSize);

						if (columnCount < 1)
							columnCount = 1;

						ImGui::Columns(columnCount, nullptr, false);

						if (animation.HasFrame())
						{
							for (int i = 0; i < animation.GetTotalFrames(); i++)
							{
								const ImTextureID animTexture = reinterpret_cast<ImTextureID>(animation.GetSprites(i)->GetRendererID());
								ImGui::Image(animTexture, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

								ImGui::TextWrapped("Frame %d", i + 1);

								ImGui::NextColumn();
							}
							ImGui::Columns();
						}

						if (state.HasAnimation())
						{
							for (int i = 0; i < state.GetAnimation().GetTotalFrames(); i++)
							{
								const ImTextureID animTexture = reinterpret_cast<ImTextureID>(state.GetAnimation().GetSprites(i)->GetRendererID());
								ImGui::Image(animTexture, ImVec2(imageSize, imageSize), ImVec2(0, 1), ImVec2(1, 0));

								const int currentIndex = state.GetAnimation().GetFrameIndex();

								if (currentIndex == i)
									ImGui::TextWrapped("  -----");

								ImGui::NextColumn();
							}
							ImGui::Columns();

							bool looping = state.IsLooping();
							if (ImGui::Checkbox("Loop", &looping))
								state.SetLooping(looping);
							ImGui::SameLine();
							if (ImGui::Button("Preview"))
								state.Preview = !state.Preview;
							ImGui::SameLine();
							if (ImGui::Button("Delete State"))
								state.RemoveState(currentState);
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
			const char* currentCMSTypeString = CMSTypeString[static_cast<int>(m_EditorCamera.GetStyle())];
			ImGui::Text("Distance %.5f", m_EditorCamera.GetDistance());

			if (ImGui::BeginCombo("CAMERA STYLE", currentCMSTypeString)) {
				for (int i = 0; i < 2; i++) {
					const bool isSelected = currentCMSTypeString == CMSTypeString[i];
					if (ImGui::Selectable(CMSTypeString[i], isSelected)) {
						currentCMSTypeString = CMSTypeString[i];
						m_EditorCamera.SetStyle(static_cast<CameraStyle>(i));

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
