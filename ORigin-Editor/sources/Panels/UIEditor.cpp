#include "UIEditor.h"
#include "../EditorLayer.h"
#include "Origin/GUI/UI.h"
#include "Origin/Renderer/Renderer2D.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Scene/Components/Components.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

namespace origin
{
	static UIEditor *s_Instance = nullptr;

	UIEditor::UIEditor(Scene *scene)
		: m_Scene(scene)
	{
		s_Instance = this;

		m_Camera.InitOrthographic(10.0f, 0.1f, 10.0f);
		m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));

		FramebufferSpecification spec;
		spec.Attachments =
		{
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::DEPTH24STENCIL8
		};

		spec.Width = 1280;
		spec.Height = 720;

		m_Framebuffer = Framebuffer::Create(spec);
	}

	void UIEditor::SetContext(Scene *scene)
	{
		if (m_UICompHandler)
		{
			m_UICompHandler = nullptr;
		}

		if (IsOpened)
			IsOpened = false;

		m_Scene = scene;
	}

	void UIEditor::SetActive(UIComponent *component)
	{
		if (!m_Scene)
			return;

		if (!IsOpened)
		{
			Entity cam = m_Scene->GetPrimaryCameraEntity();
			if (cam.IsValid())
			{
				auto &cc = cam.GetComponent<CameraComponent>();
				const float orthoSizeY = cc.Camera.GetOrthoSize().y * 2.0f;
				m_Camera.SetOrthoScale(orthoSizeY * 1.3f);
				m_Camera.SetOrthoScaleMax(orthoSizeY * 4.0f);
			}
			m_UICompHandler = component;
			IsOpened = true;

			ImGui::SetWindowFocus("UI Editor");
		}
	}

	void UIEditor::CreateNewText()
	{
		if (!m_UICompHandler)
			return;

		UIData<TextComponent> component;
		component.Component.TextString = "This is text component";
		std::string defaultName = "Text";
		m_UICompHandler->AddComponent<TextComponent>(defaultName, component);
		m_SelectedIndex = m_UICompHandler->Components.size() - 1;
	}

	void UIEditor::CreateNewTexture()
	{
		if (!m_UICompHandler)
			return;

		UIData<SpriteRenderer2DComponent> component;
		m_UICompHandler->AddComponent<SpriteRenderer2DComponent>("Sprite", component);
		m_SelectedIndex = m_UICompHandler->Components.size() - 1;
	}

	bool UIEditor::RenameComponent(int index, const std::string &newName)
	{
		if (m_UICompHandler)
			return m_UICompHandler->RenameComponent(index, newName);
		return false;
	}

	void UIEditor::OnImGuiRender()
	{
		if (IsOpened)
		{
			ImGui::Begin("UI Editor", &IsOpened,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			IsViewportFocused = ImGui::IsWindowFocused();
			IsViewportHovered = ImGui::IsWindowHovered();

			const ImVec2 &viewportMinRegion = ImGui::GetWindowContentRegionMin();
			const ImVec2 &viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			const ImVec2 &viewportOffset = ImGui::GetWindowPos();

			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
			m_EditorViewportSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

			// Framebuffer Texture
			ImTextureID texture = reinterpret_cast<ImTextureID>(m_Framebuffer->GetColorAttachmentRendererID());
			ImGui::Image(texture, { m_EditorViewportSize.x, m_EditorViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();

			DrawInspector();
			DrawHierarchy();
		}
	}

	void UIEditor::DrawInspector()
	{
		ImGui::Begin("UI Inspector");

		if (m_UICompHandler)
		{
			for (int i = 0; i < m_UICompHandler->Components.size(); i++)
			{
				if (m_SelectedIndex == i)
				{
					if (UIData<TextComponent> *text = m_UICompHandler->GetComponent<TextComponent>(m_UICompHandler->Components[i]->Name))
					{
						std::string name = text->Name;

						char buffer[256];
						strncpy(buffer, name.c_str(), sizeof(buffer));
						if (ImGui::InputText("##Tag", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
						{
							std::string newName(buffer);
							if (!newName.empty() && newName != name)
							{
								m_UICompHandler->RenameComponent(m_SelectedIndex, newName);
							}
						}

						UI::DrawVec3Control("Translation", text->Transform.WorldTranslation, 0.1f);
						glm::vec3 eulerRotation = glm::degrees(glm::eulerAngles(text->Transform.WorldRotation));
						UI::DrawVec3Control("Rotation", eulerRotation, 0.1f);
						glm::vec3 rotationRadians = glm::radians(eulerRotation);
						text->Transform.WorldRotation = glm::quat(rotationRadians);
						UI::DrawVec3Control("Scale", text->Transform.WorldScale, 0.1f, 1.0f);

						UI::DrawButtonWithColumn("Font", "Drag Here", nullptr, [&]()
						{
							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
								{
									AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
									if (AssetManager::GetAssetType(handle) == AssetType::Font)
									{
										text->Component.FontHandle = handle;
									}
									else
									{
										OGN_CORE_WARN("[UIEditor] Wrong asset type!");
										PUSH_CONSOLE_WARNING("[UIEditor] Wrong asset type!");
									}
								}
								ImGui::EndDragDropTarget();
							}

							if (text->Component.FontHandle)
							{
								ImGui::SameLine();
								if (UI::DrawButton("X"))
								{
									text->Component.FontHandle = 0;
								}
							}
						});


						if (text->Component.FontHandle)
						{
							ImGui::InputTextMultiline("Text String", &text->Component.TextString);
							ImGui::ColorEdit4("Color", glm::value_ptr(text->Component.Color));
							UI::DrawFloatControl("Kerning", &text->Component.Kerning, 0.01f);
							UI::DrawFloatControl("Line Spacing", &text->Component.LineSpacing, 0.01f);
						}
					}
					else if (UIData<SpriteRenderer2DComponent> *sprite = m_UICompHandler->GetComponent<SpriteRenderer2DComponent>(m_UICompHandler->Components[i]->Name))
					{
						std::string name = sprite->Name;

						char buffer[256];
						strncpy(buffer, name.c_str(), sizeof(buffer));
						if (ImGui::InputText("##Tag", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
						{
							std::string newName(buffer);
							if (!newName.empty() && newName != name)
							{
								m_UICompHandler->RenameComponent(m_SelectedIndex, newName);
							}
						}

						UI::DrawVec3Control("Translation", sprite->Transform.WorldTranslation, 0.1f);
						glm::vec3 eulerRotation = glm::degrees(glm::eulerAngles(sprite->Transform.WorldRotation));
						UI::DrawVec3Control("Rotation", eulerRotation, 0.1f);
						glm::vec3 rotationRadians = glm::radians(eulerRotation);
						sprite->Transform.WorldRotation = glm::quat(rotationRadians);
						UI::DrawVec3Control("Scale", sprite->Transform.WorldScale, 0.1f, 1.0f);

						ImGui::ColorEdit4("Color", glm::value_ptr(sprite->Component.Color));

						std::string lable = "None";
						if (sprite->Component.Texture != 0)
						{
							if (AssetManager::IsAssetHandleValid(sprite->Component.Texture) && AssetManager::GetAssetType(sprite->Component.Texture) == AssetType::Texture)
							{
								const AssetMetadata &metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(sprite->Component.Texture);
								lable = metadata.Filepath.filename().string();
							}
							else
							{
								lable = "Invalid";
							}
						}

						UI::DrawButtonWithColumn(lable.c_str(), "Texture", nullptr, [&]()
						{
							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
								{
									AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
									if (AssetManager::GetAssetType(handle) == AssetType::Texture)
									{
										sprite->Component.Texture = handle;
										sprite->Component.Min = glm::vec2(0.0f);
										sprite->Component.Max = glm::vec2(1.0f);
									}
									else
									{
										OGN_CORE_WARN("[UIEditor] Wrong asset type!");
										PUSH_CONSOLE_WARNING("[UIEditor] Wrong asset type!");
									}
								}
								else if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SPRITESHEET_ITEM"))
								{
									SpriteSheetData data = *static_cast<SpriteSheetData *>(payload->Data);
									sprite->Component.Texture = data.TextureHandle;
									sprite->Component.Min = data.Min;
									sprite->Component.Max = data.Max;
								}

								ImGui::EndDragDropTarget();
							}
							if (sprite->Component.Texture)
							{
								ImGui::SameLine();
								if (UI::DrawButton("X"))
								{
									sprite->Component.Texture = 0;
									sprite->Component.Min = glm::vec2(0.0f);
									sprite->Component.Max = glm::vec2(1.0f);
								}
							}
						});

						if (sprite->Component.Texture)
						{
							UI::DrawVec2Control("Tilling Factor", sprite->Component.TillingFactor, 0.025f, 1.0f);
							UI::DrawCheckbox("Flip X", &sprite->Component.FlipX);
							UI::DrawCheckbox("Flip Y", &sprite->Component.FlipY);
						}
					}
				}
			}
		}

		ImGui::End();
	}

	void UIEditor::DrawHierarchy()
	{
		ImGui::Begin("UI Hierarchy");

		if (ImGui::BeginPopupContextWindow("CreateUI",
			ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Text"))
				UIEditor::CreateNewText();
			if (ImGui::MenuItem("Sprite"))
				UIEditor::CreateNewTexture();
			ImGui::EndPopup();
		}

		if (m_UICompHandler)
		{
			for (int i = 0; i < m_UICompHandler->Components.size(); i++)
			{
				ImGuiTreeNodeFlags flags = (m_SelectedIndex == i ? ImGuiTreeNodeFlags_Selected : 0)
					| ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow
					| ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

				if (UIData<TextComponent> *text = m_UICompHandler->GetComponent<TextComponent>(m_UICompHandler->Components[i]->Name))
				{
					ImGui::PushID(text->Name.c_str());

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 0.5f, 2.0f });
					bool node_open = ImGui::TreeNodeEx((void *)&text, flags, text->Name.c_str());
					ImGui::PopStyleVar();

					bool isDeleting = false;
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete"))
						{
							isDeleting = true;
						}
						ImGui::EndPopup();
					}

					if (ImGui::IsItemHovered())
					{
						if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							m_SelectedIndex = i;
						}
					}

					if (node_open)
					{
						ImGui::TreePop();
					}

					if (isDeleting)
					{
						m_UICompHandler->RemoveComponent(i);
						i--;
					}

					ImGui::PopID();

				}

				else if (UIData<SpriteRenderer2DComponent> *sprite = m_UICompHandler->GetComponent<SpriteRenderer2DComponent>(m_UICompHandler->Components[i]->Name))
				{
					ImGui::PushID(sprite->Name.c_str());

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 0.5f, 2.0f });
					bool node_open = ImGui::TreeNodeEx((void *)&sprite, flags, sprite->Name.c_str());
					ImGui::PopStyleVar();

					bool isDeleting = false;
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete"))
						{
							isDeleting = true;
						}
						ImGui::EndPopup();
					}

					if (ImGui::IsItemHovered())
					{
						if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
						{
							m_SelectedIndex = i;
						}
					}

					if (node_open)
					{
						ImGui::TreePop();
					}

					if (isDeleting)
					{
						m_UICompHandler->RemoveComponent(i);
						i--;
					}

					ImGui::PopID();
				}
			}
		}

		ImGui::End();
	}

	void UIEditor::OnUpdate(Timestep ts)
	{
		if (!IsOpened)
		{
			if (m_UICompHandler)
			{
				m_UICompHandler = nullptr;
			}
			if (m_Scene)
			{
				m_Scene = nullptr;
			}

			return;
		}

		m_Camera.SetAllowedMove(IsViewportFocused && IsViewportHovered && !ImGui::GetIO().WantTextInput);
		m_Camera.OnUpdate(ts, m_ViewportBounds[0], m_ViewportBounds[1]);
		OnMouse(ts);

		m_Framebuffer->Bind();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_Framebuffer->ClearAttachment(1, -1);

		if (m_UICompHandler != nullptr)
		{
			if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification();
					m_EditorViewportSize.x > 0.0f && m_EditorViewportSize.y > 0.0f && (m_EditorViewportSize.x != spec.Width || m_EditorViewportSize.y != spec.Height))
			{
				m_Camera.SetViewportSize(m_EditorViewportSize.x, m_EditorViewportSize.y);
				m_Framebuffer->Resize(static_cast<uint32_t>(m_EditorViewportSize.x), static_cast<uint32_t>(m_EditorViewportSize.y));
			}

			Renderer2D::Begin(m_Camera);

			// Draw Camera Boundary
			Entity cam = m_Scene->GetPrimaryCameraEntity();
			if (cam.IsValid())
			{
				auto &cc = cam.GetComponent<CameraComponent>();
				const glm::vec2 &orthoSize = cc.Camera.GetOrthoSize();
				Renderer2D::DrawRect(glm::scale(glm::mat4(1.0f), { orthoSize.x * 2.0f, orthoSize.y * 2.0f, 1.0f }), { 0.5f, 0.5f, 0.5f, 1.0f });

				for (int i = 0; i < m_UICompHandler->Components.size(); i++)
				{
					if (UIData<TextComponent> *text = m_UICompHandler->GetComponent<TextComponent>(m_UICompHandler->Components[i]->Name))
					{
						if (text->Component.FontHandle)
						{
							Renderer2D::DrawString(text->Component.TextString, text->Transform.GetTransform(), text->Component, i);
						}
					}
					else if (UIData<SpriteRenderer2DComponent> *sprite = m_UICompHandler->GetComponent<SpriteRenderer2DComponent>(m_UICompHandler->Components[i]->Name))
					{
						Renderer2D::DrawSprite(sprite->Transform.GetTransform(), sprite->Component, i);
						if (m_SelectedIndex == i)
						{
							glLineWidth(2.0f);
							Renderer2D::DrawRect(sprite->Transform.GetTransform(), glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
						}
					}
				}
			}

			Renderer2D::End();
			glLineWidth(1.0);
		}

		if (IsViewportHovered && IsViewportFocused)
		{
			auto [mx, my] = ImGui::GetMousePos();
			m_Mouse = { mx, my };
			m_Mouse -= m_ViewportBounds[0];
			const glm::ivec2 &viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
			m_Mouse.y = viewportSize.y - m_Mouse.y;
			m_Mouse = glm::clamp(m_Mouse, { 0, 0 }, viewportSize - glm::ivec2 { 1, 1 });
			m_HoveredIndex = m_Framebuffer->ReadPixel(1, m_Mouse.x, m_Mouse.y);
		}

		m_Framebuffer->Unbind();
	}

	void UIEditor::OnEvent(Event &e)
	{
		OGN_PROFILER_INPUT();

		m_Camera.OnEvent(e);
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(UIEditor::OnMouseButtonPressed));
		dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(UIEditor::OnKeyPressed));
	}

	bool UIEditor::OnMouseButtonPressed(MouseButtonPressedEvent &e)
	{
		if (e.Is(Mouse::ButtonLeft) && IsViewportHovered)
		{
			if (m_HoveredIndex != (m_SelectedIndex == 0 ? -1 : m_SelectedIndex) && m_HoveredIndex >= 0)
			{
				m_SelectedIndex = m_HoveredIndex;
			}
			else if (m_HoveredIndex <= -1)
			{
				m_SelectedIndex = -1;
			}
		}
		
		if (e.Is(Mouse::ButtonMiddle) && IsViewportHovered)
		{
			ImGui::SetWindowFocus("UI Editor");
		}

		return false;
	}

	bool UIEditor::OnKeyPressed(KeyPressedEvent &e)
	{
		return false;
	}

	void UIEditor::OnMouse(float ts)
	{
		const glm::vec2 mouse { Input::Get().GetMouseX(), Input::Get().GetMouseY() };
		const glm::vec2 delta = Input::Get().GetMouseDelta();

		if (!m_UICompHandler)
			return;

		if (Input::Get().IsMouseButtonPressed(Mouse::ButtonLeft) && IsViewportHovered)
		{
			for (int i = 0; i < m_UICompHandler->Components.size(); i++)
			{
				if (m_SelectedIndex == i)
				{
					auto &tc = m_UICompHandler->Components[m_SelectedIndex]->Transform;
					float viewportHeight = m_Camera.GetViewportSize().y;
					float orthoScale = m_Camera.GetOrthoScale() / viewportHeight;

					static glm::vec3 translation = tc.Translation;

					if (Input::Get().IsKeyPressed(Key::LeftShift))
					{
						float snapeValue = 0.5f;
						if (Input::Get().IsKeyPressed(Key::LeftControl))
							snapeValue = 0.1f;

						translation += glm::vec3(delta.x * orthoScale, -delta.y * orthoScale, 0.0f);
						tc.WorldTranslation.x = round(translation.x / snapeValue) * snapeValue;
						tc.WorldTranslation.y = round(translation.y / snapeValue) * snapeValue;
					}
					else
					{
						translation = glm::vec3(delta.x * orthoScale, -delta.y * orthoScale, 0.0f);
						tc.WorldTranslation += glm::vec3(glm::vec2(translation), 0.0f);
						translation = tc.WorldTranslation;
					}
				}
			}
		}
	}

	void UIEditor::Open()
	{
		IsOpened = true;
		ImGui::SetWindowFocus("UI Editor");
	}

	UIEditor *UIEditor::Get()
	{
		return s_Instance;
	}

}
