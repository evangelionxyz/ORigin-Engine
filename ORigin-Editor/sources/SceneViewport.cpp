// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "Editor.h"

namespace Origin
{
	extern const std::filesystem::path g_AssetPath;

	const char* Editor::MenuContextToString(const ViewportMenuContext& context)
	{
		switch (context)
		{
			case CreateMenu: return "Create Menu"; break;
			case EntityProperties: return "Entity Properties"; break;
		}

		return nullptr;
	}

	void Editor::SceneViewport()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_NoCollapse;

		ImGui::Begin("Scene", nullptr, window_flags);

		m_SceneViewportHovered = ImGui::IsWindowHovered();
		m_SceneViewportFocused = ImGui::IsWindowFocused();

		m_SceneHierarchy.SetHierarchyMenuActive(!ImGui::IsWindowFocused());

		SceneViewportMenu();
		SceneViewportToolbar();

		ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
		ImVec2& viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2& viewportOffset = ImGui::GetWindowPos();
		m_SceneViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_SceneViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		// Debug Info Overlay
		if (guiOverlay)
		{
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
				| ImGuiWindowFlags_AlwaysAutoResize
				| ImGuiWindowFlags_NoSavedSettings
				| ImGuiWindowFlags_NoFocusOnAppearing
				| ImGuiWindowFlags_NoNav;

			ImGui::SetNextWindowPos(
				{ (viewportMinRegion.x + viewportOffset.x) + 8.0f,
				(viewportMinRegion.y + viewportOffset.y) + 8.0f }, ImGuiCond_Always);

			ImGui::SetNextWindowBgAlpha(0.0f); // Transparent background
			if (ImGui::Begin("##top_left_overlay", nullptr, window_flags))
			{
				if (!m_SceneHierarchy.GetContext())
					ImGui::Text("Load a Scene or Create New Scene to begin!");
				ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::Text("Gizmo Type : %i", m_GizmosType);
				std::string name = "None";
				if (m_HoveredEntity) name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
				ImGui::Text("Hovered Entity: (%s) (%d)", name.c_str(), m_PixelData);
			}
			ImGui::End();
		}

		ImVec2& viewportPanelSize = ImGui::GetContentRegionAvail();
		m_SceneViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		uint64_t viewportID = m_Framebuffer->GetColorAttachmentRendererID(m_RenderTarget);
		ImGui::Image(reinterpret_cast<ImTextureID*>(viewportID), ImVec2(m_SceneViewportSize.x, m_SceneViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
			{
				if (m_SceneState == SceneState::Edit)
				{
					if (m_HoveredEntity)
					{
						m_SceneHierarchy.SetSelectedEntity(m_HoveredEntity);
						auto entity = m_SceneHierarchy.GetSelectedEntity();
						if (entity.HasComponent<SpriteRendererComponent>())
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path textureFile = std::filesystem::path(g_AssetPath) / path;
							auto& component = entity.GetComponent<SpriteRendererComponent>();
							if (textureFile.extension() == ".png" || textureFile.extension() == ".jpg")
								component.Texture = Texture2D::Create(textureFile.string());
						}
					}
					else
					{
						const auto* path = static_cast<const wchar_t*>(payload->Data);
						auto scenePath = std::filesystem::path(g_AssetPath) / path;
						if (scenePath.extension() == ".org" || scenePath.extension() == ".origin")
							OpenScene(scenePath);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (m_SelectedEntity && m_GizmosType != -1 && m_GizmosActive)
		{
			// Gizmos
			ImGuizmo::SetOrthographic(false); // by default is false
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(
				m_SceneViewportBounds[0].x, m_SceneViewportBounds[0].y,
				m_SceneViewportBounds[1].x - m_SceneViewportBounds[0].x,
				m_SceneViewportBounds[1].y - m_SceneViewportBounds[0].y
			);

			// Editor Camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();
			glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

			auto& tc = m_SelectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();
			glm::vec3 originalRotation = tc.Rotation;

			bool snap = Input::IsKeyPressed(Key::LeftShift);
			float snapValue = 0.5f;

			if (snap && m_GizmosType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(
				glm::value_ptr(cameraView),
				glm::value_ptr(cameraProjection),
				static_cast<ImGuizmo::OPERATION>(m_GizmosType),
				static_cast<ImGuizmo::MODE>(m_GizmosMode),
				glm::value_ptr(transform),
				nullptr,
				snap ? snapValues : nullptr
			);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				tc.Translation = translation;
				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		if (ImGui::IsWindowFocused() && Input::IsKeyPressed(Key::Escape)) m_GizmosType = -1;
		m_EditorCamera.EnableMovement(m_SceneViewportHovered && !ImGuizmo::IsUsing());

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void Editor::SceneViewportToolbar()
	{
		ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
		ImVec2& viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2& viewportOffset = ImGui::GetWindowPos();

		const float& wndWidth = ImGui::GetWindowWidth();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoScrollWithMouse
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoCollapse;

		float wndYpos = { (viewportMinRegion.y + viewportOffset.y) + 4.0f };

		// Play Button
		ImGui::SetNextWindowPos({ (viewportMinRegion.x + viewportOffset.x) + wndWidth / 2.0f, wndYpos }, ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.0f);

		if (ImGui::Begin("##play_button", nullptr, window_flags))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 2.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0.0f, 0.0f));

			{
				std::shared_ptr<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_PlayButton : m_StopButton;
				ImGui::SameLine(viewportMinRegion.x * 0.5f);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
				if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(icon->GetRendererID()), ImVec2(25.0f, 25.0f)))
				{
					if (m_SceneHierarchy.GetContext())
					{
						if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
						{
							m_HoveredEntity = {};
							OnScenePlay();
						}
						else if (m_SceneState == SceneState::Play)
						{
							m_HoveredEntity = {};
							OnSceneStop();
						}
					}
				}
				ImGui::PopStyleColor(3);
			}

			{
				std::shared_ptr<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_SimulateButton : m_StopButton;
				ImGui::SameLine(viewportMinRegion.x * 0.5f);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.3f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
				if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(icon->GetRendererID()), ImVec2(25.0f, 25.0f)))
				{
					if (m_SceneHierarchy.GetContext())
					{
						if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
						{
							m_HoveredEntity = {};
							OnSceneSimulate();
						}
						else if (m_SceneState == SceneState::Simulate)
						{
							m_HoveredEntity = {};
							OnSceneStop();
						}
					}
				}
				ImGui::PopStyleColor(3);
			}

			ImGui::PopStyleVar(2);
			ImGui::End(); // !viewport_toolbar
		}

		// Guizmo MODE
		ImGui::SetNextWindowPos({ (viewportMinRegion.x + viewportOffset.x) + wndWidth - 120.0f, wndYpos }, ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.0f);
		if (ImGui::Begin("##gizmo_mode", nullptr, window_flags))
		{
			ImVec4 btActive = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
			ImVec4 btLOCAL, btGLOBAL;
			if (m_GizmosMode == ImGuizmo::MODE::LOCAL)
			{
				btGLOBAL = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);
				btLOCAL = btActive;
			}

			if (m_GizmosMode == ImGuizmo::MODE::WORLD)
			{
				btLOCAL = ImVec4(0.1f, 0.2f, 0.8f, 1.0f);
				btGLOBAL = btActive;
			}

			ImGui::PushStyleColor(ImGuiCol_Button, btLOCAL);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btLOCAL);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, btActive);
			if (ImGui::Button("LOCAL", { 50.0f, 25.0f }))
				m_GizmosMode = ImGuizmo::MODE::LOCAL;
			ImGui::PopStyleColor(3);
			ImGui::SameLine(0.0f, 5.0f);

			ImGui::PushStyleColor(ImGuiCol_Button, btGLOBAL);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, btGLOBAL);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, btActive);
			if (ImGui::Button("GLOBAL", { 50.0f, 25.0f }))
				m_GizmosMode = ImGuizmo::MODE::WORLD;
			ImGui::PopStyleColor(3);

			ImGui::End(); // !gizmo_mode
		}
	}

	void Editor::SceneViewportMenu()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		if (VpMenuContextActive && m_SceneHierarchy.GetContext())
		{
			if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
			{
				// Create Menu
				if (m_VpMenuContext == ViewportMenuContext::CreateMenu)
				{
					ImGui::Text("Add"); ImGui::Separator();

					if (ImGui::MenuItem("Empty")) m_SceneHierarchy.GetContext()->CreateEntity("Empty");
					if (ImGui::MenuItem("Camera")) m_SceneHierarchy.GetContext()->CreateCamera("Camera");
					ImGui::Separator();

					ImGui::Text("2D"); ImGui::Separator();
					if (ImGui::MenuItem("Sprite")) m_SceneHierarchy.GetContext()->CreateSpriteEntity("Sprite");
					if (ImGui::MenuItem("Circle")) m_SceneHierarchy.GetContext()->CreateCircle("Circle");
				}

				// Entity Properties
				if (m_VpMenuContext == ViewportMenuContext::EntityProperties)
				{
					// Entity Properties
					std::string name = "None";
					m_SelectedEntity == m_SceneHierarchy.GetSelectedEntity() ?
						name = m_SelectedEntity.GetComponent<TagComponent>().Tag : name;

					ImGui::Text("%s", name.c_str());
					ImGui::Separator();

					if (ImGui::BeginMenu("Properties"))
					{
						ImGui::Text("Rename");
						if (m_SelectedEntity.HasComponent<TagComponent>())
						{
							auto& tag = m_SelectedEntity.GetComponent<TagComponent>().Tag;
							char buffer[64];
							strcpy_s(buffer, sizeof(buffer), tag.c_str());
							if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
							{
								tag = std::string(buffer);
								if (tag.empty()) tag = "'No Name'";
							}
						}

						if (m_SelectedEntity.HasComponent<SpriteRendererComponent>())
						{
							auto& component = m_SelectedEntity.GetComponent<SpriteRendererComponent>();
							ImGui::Separator();

							ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
							if (component.Texture)
							{
								ImGui::Text("Texture");
								ImGui::DragFloat("Tilling Factor", &component.TillingFactor, 0.1f, 0.0f, 10.0f);
								if (ImGui::Button("Delete", ImVec2(64.0f, 24.0f)))
								{
									component.Texture->Delete();
									component.Texture = {};
								}
							}
						}

						if (m_SelectedEntity.HasComponent<CircleRendererComponent>())
						{
							auto& component = m_SelectedEntity.GetComponent<CircleRendererComponent>();
							ImGui::Separator();
							ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
							ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
							ImGui::DragFloat("Fade", &component.Fade, 0.025f, 0.0f, 1.0f);
						}
						ImGui::EndMenu(); //!Properties
					}
				}
				ImGui::EndPopup();
			}
		}
		ImGui::PopStyleVar();
	}
}