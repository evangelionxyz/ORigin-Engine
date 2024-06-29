#include "UIEditor.h"
#include "..\EditorLayer.h"
#include "Origin\Renderer\Renderer2D.h"
#include "Origin\Asset\AssetManager.h"
#include "Origin\GUI\UI.h"

#include <imgui.h>
#include <misc\cpp\imgui_stdlib.h>

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
				const float orthoSizeY = cc.Camera.GetOrthographicSize().y;
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

	bool UIEditor::RenameComponent(const std::string &oldKey, const std::string &newKeyBase)
	{
		if (m_UICompHandler)
			return m_UICompHandler->RenameComponent(oldKey, newKeyBase);
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
			int i = 0;
			for (auto &[key, ui] : m_UICompHandler->Components)
			{
				if (m_SelectedIndex == i)
				{
					if (m_UICompHandler->Is<TextComponent>(key))
					{
						UIData<TextComponent> *text = m_UICompHandler->GetComponent<TextComponent>(key);
						std::string nameBuffer = key;
						auto &name = nameBuffer;

						char buffer[256];
						strcpy_s(buffer, sizeof(buffer), name.c_str());
						if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
						{
							//name = std::string(buffer);
							//if (name.empty())
							//  name = "'No Name'";
						}

						UI::DrawVec3Control("Translation", text->Transform.WorldTranslation, 0.1f);
						glm::vec3 rotation = glm::degrees(text->Transform.WorldRotation);
						UI::DrawVec3Control("Rotation", rotation, 0.1f);
						text->Transform.WorldRotation = glm::radians(rotation);
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
									}
								}
							}

							if (text->Component.FontHandle)
							{
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
					else if (m_UICompHandler->Is<SpriteRenderer2DComponent>(key))
					{
						UIData<SpriteRenderer2DComponent> *sprite = m_UICompHandler->GetComponent<SpriteRenderer2DComponent>(key);
						std::string nameBuffer = key;
						auto &name = nameBuffer;

						char buffer[256];
						strcpy_s(buffer, sizeof(buffer), name.c_str());
						if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
						{
							// name = std::string(buffer);
							// if (name.empty())
							//	name = "'No Name'";
						}

						UI::DrawVec3Control("Translation", sprite->Transform.WorldTranslation, 0.1f);
						glm::vec3 rotation = glm::degrees(sprite->Transform.WorldRotation);
						UI::DrawVec3Control("Rotation", rotation, 0.1f);
						sprite->Transform.WorldRotation = glm::radians(rotation);
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
				i++;
			}
		}

		ImGui::End();
	}

	void UIEditor::DrawHierarchy()
	{
		ImGui::Begin("UI Hierarchy");

		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		ImGui::SameLine(contentRegionAvailable.x - 24.0f);
		ImTextureID texId = reinterpret_cast<ImTextureID>(EditorLayer::Get().m_UITextures.at("plus")->GetRendererID());

		if (ImGui::ImageButton(texId, ImVec2(14.0f, 14.0f)))
			ImGui::OpenPopup("CreateUI");

		if (ImGui::BeginPopup("CreateUI"))
		{
			if (ImGui::MenuItem("Text"))
				UIEditor::CreateNewText();
			if (ImGui::MenuItem("Sprite"))
				UIEditor::CreateNewTexture();

			ImGui::EndPopup();
		}

		if (m_UICompHandler)
		{
			int i = 0;
			for (auto &[key, value] : m_UICompHandler->Components)
			{
				ImGuiTreeNodeFlags flags = (m_SelectedIndex == i ? ImGuiTreeNodeFlags_Selected : 0)
					| ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow
					| ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

				if (m_UICompHandler->Is<TextComponent>(key))
				{
					ImGui::PushID(key.c_str());

					UIData<TextComponent> *text = m_UICompHandler->GetComponent<TextComponent>(key);

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 0.5f, 2.0f });
					bool node_open = ImGui::TreeNodeEx((void *)&text, flags, key.c_str());
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
						m_UICompHandler->RemoveComponent(key);
						break;
					}
					ImGui::PopID();
				}
				else if (m_UICompHandler->Is<SpriteRenderer2DComponent>(key))
				{
					ImGui::PushID(key.c_str());

					UIData<SpriteRenderer2DComponent> *sprite = m_UICompHandler->GetComponent<SpriteRenderer2DComponent>(key);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 0.5f, 2.0f });
					bool node_open = ImGui::TreeNodeEx((void *)&sprite, flags, key.c_str());
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
						m_UICompHandler->RemoveComponent(key);
						break;
					}

					ImGui::PopID();
				}

				i++;
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

		m_Camera.SetMoveActive(IsViewportFocused);
		m_Camera.SetDraggingActive(IsViewportFocused);
		m_Camera.SetScrollingActive(IsViewportHovered);

		m_Camera.OnUpdate(ts);
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
				const glm::vec2 &orthoSize = cc.Camera.GetOrthographicSize();
				Renderer2D::DrawRect(glm::scale(glm::mat4(1.0f), { orthoSize.x, orthoSize.y, 1.0f }), { 0.5f, 0.5f, 0.5f, 1.0f });

				int i = 0;
				for (auto &[key, value] : m_UICompHandler->Components)
				{
					if (m_UICompHandler->Is<TextComponent>(key))
					{
						UIData<TextComponent> *text = m_UICompHandler->GetComponent<TextComponent>(key);
						if (text->Component.FontHandle)
						{
							Renderer2D::DrawString(text->Component.TextString, text->Transform.GetTransform(), text->Component, i);
						}
					}
					else if (m_UICompHandler->Is<SpriteRenderer2DComponent>(key))
					{
						UIData<SpriteRenderer2DComponent> *sprite = m_UICompHandler->GetComponent<SpriteRenderer2DComponent>(key);
						Renderer2D::DrawSprite(sprite->Transform.GetTransform(), sprite->Component, i);
					}

					i++;
				}
			}

			Renderer2D::End();
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
		if (e.Is(Mouse::ButtonLeft))
		{
			if (m_HoveredIndex != (m_SelectedIndex == 0 ? -1 : m_SelectedIndex) && m_HoveredIndex >= 0)
				m_SelectedIndex = m_HoveredIndex;
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
