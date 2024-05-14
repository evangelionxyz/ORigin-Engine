#include "UIEditor.h"
#include "..\EditorLayer.h"
#include "Origin\Renderer\RenderCommand.h"
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
		if (m_Component)
			m_Component = nullptr;

		m_Scene = scene;
	}

	void UIEditor::SetActive(UIComponent *component)
	{
		if (!IsOpened)
		{
			if (Entity pc = m_Scene->GetPrimaryCameraEntity())
			{
				auto &cc = pc.GetComponent<CameraComponent>().Camera;
				m_MainCamSize = cc.GetViewportSize();
				m_Component = component;

				m_Camera.SetOrthoSize(m_MainCamSize.y * 1.2f);
				m_Camera.SetOrthoSizeMax(m_MainCamSize.y * 5.0f);
			}

			IsOpened = true;
		}
	}

	void UIEditor::CreateNewText()
	{
		UIData<TextComponent> text;
		text.Name = "Text";
		text.Component.TextString = "Text";

		m_Component->Texts.push_back(text);
		m_SelectedIndex = m_Component->Texts.size() - 1;
	}

	void UIEditor::OnImGuiRender()
	{
		if (IsOpened)
		{
			ImGui::Begin("UI Editor", &IsOpened,
				ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			IsFocused = ImGui::IsWindowFocused();
			IsHovered = ImGui::IsWindowHovered();

			m_Camera.SetMoveActive(IsFocused);
			m_Camera.SetDraggingActive(IsFocused);
			m_Camera.SetScrollingActive(IsHovered);

			const ImVec2 &viewportMinRegion = ImGui::GetWindowContentRegionMin();
			const ImVec2 &viewportMaxRegion = ImGui::GetWindowContentRegionMax();
			const ImVec2 &viewportOffset = ImGui::GetWindowPos();

			m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
			m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
			m_ViewportSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y };

			// Framebuffer Texture
			ImTextureID texture = reinterpret_cast<ImTextureID>(m_Framebuffer->GetColorAttachmentRendererID());
			ImGui::Image(texture, { m_ViewportSize.x, m_ViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();

			ImGui::Begin("UI Inspector");

			if (m_Component)
			{
				for (int i = 0; i < m_Component->Texts.size(); i++)
				{
					if (m_SelectedIndex == i)
					{
						auto &text = m_Component->Texts[i];
						auto &name = text.Name;

						char buffer[256];
						strcpy_s(buffer, sizeof(buffer), name.c_str());
						if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
						{
							name = std::string(buffer);
							if (name.empty()) name = "'No Name'";
						}

						UI::DrawVec3Control("Translation", text.Transform.WorldTranslation, 0.5f);
						glm::vec3 rotation = glm::degrees(text.Transform.WorldRotation);
						UI::DrawVec3Control("Rotation", rotation, 0.5f);
						text.Transform.Rotation = glm::radians(rotation);
						UI::DrawVec3Control("Scale", text.Transform.WorldScale, 0.5f, 1.0f);

						ImGui::Button("DROP FONT", ImVec2(80.0f, 20.0f));
						if (ImGui::BeginDragDropTarget())
						{
							if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
							{
								AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
								if (AssetManager::GetAssetType(handle) == AssetType::Font) text.Component.FontHandle = handle;
							}
						}

						if (text.Component.FontHandle)
						{
							ImGui::SameLine();
							if (ImGui::Button("X")) text.Component.FontHandle = 0;
						}

						if (text.Component.FontHandle != 0)
						{
							ImGui::InputTextMultiline("Text String", &text.Component.TextString);
							ImGui::ColorEdit4("Color", glm::value_ptr(text.Component.Color));
							UI::DrawVecControl("Kerning", &text.Component.Kerning, 0.01f);
							UI::DrawVecControl("Line Spacing", &text.Component.LineSpacing, 0.01f);
						}
					}
				}
			}

			ImGui::End();

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
				ImGui::EndPopup();
			}

			if (m_Component)
			{
				for (int i = 0; i < m_Component->Texts.size(); i++)
				{
					auto &text = m_Component->Texts[i];

					ImGuiTreeNodeFlags flags = (m_SelectedIndex == i ? ImGuiTreeNodeFlags_Selected : 0)
						| ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow
						| ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 0.5f, 2.0f });
					bool node_open = ImGui::TreeNodeEx((void *)&text, flags, text.Name.c_str());
					ImGui::PopStyleVar();

					bool isDeleting = false;
					if (ImGui::BeginPopupContextItem())
					{
						if (ImGui::MenuItem("Delete"))
							isDeleting = true;
						ImGui::EndPopup();
					}

					if (ImGui::IsItemHovered())
					{
						if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
							m_SelectedIndex = i;
					}

					if (node_open)
						ImGui::TreePop();

					if (isDeleting)
					{
						m_Component->Texts.erase(m_Component->Texts.begin() + i); i--;
						OGN_CORE_TRACE("UI Text erased");
					}
				}
			}

			ImGui::End();
		}
	}

	void UIEditor::OnUpdate(Timestep ts)
	{
		if (!IsOpened)
		{
			if (m_Component)
				m_Component = nullptr;

			return;
		}

		OGN_PROFILER_UI();

		m_Camera.OnUpdate(ts);
		OnMouse(ts);

		m_Framebuffer->Bind();
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_Framebuffer->ClearAttachment(1, -1);

		if (m_Component)
		{
			if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification();
					m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (m_ViewportSize.x != spec.Width || m_ViewportSize.y != spec.Height))
			{
				m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
				m_Framebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
			}
		
			Renderer2D::Begin(m_Camera);

			if (Entity pc = m_Scene->GetPrimaryCameraEntity())
			{
				auto &cc = pc.GetComponent<CameraComponent>().Camera;
				m_MainCamSize = cc.GetViewportSize();

				Renderer2D::DrawRect(glm::vec3(0.0f), m_MainCamSize * 2.0f);

				for (int i = 0; i < m_Component->Texts.size(); i++)
				{
					auto &text = m_Component->Texts[i];
					
					if (text.Component.FontHandle != 0)
						Renderer2D::DrawString(text.Component.TextString, text.Transform.GetTransform(), text.Component, i);
				}
			}

			Renderer2D::End();
		}

		auto [mx, my] = ImGui::GetMousePos();
		glm::vec2 mousePos = { mx, my };
		mousePos -= m_ViewportBounds[0];
		const glm::vec2 &viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
		mousePos.y = viewportSize.y - mousePos.y;
		mousePos = glm::clamp(mousePos, glm::vec2(0.0f), viewportSize - glm::vec2(1.0f));
		m_Mouse = { static_cast<int>(mousePos.x), static_cast<int>(mousePos.y) };
		m_HoveredIndex = m_Framebuffer->ReadPixel(1, m_Mouse.x, m_Mouse.y);

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
		if (m_HoveredIndex != (m_SelectedIndex == 0 ? -1 : m_SelectedIndex) && m_HoveredIndex >= 0)
			m_SelectedIndex = m_HoveredIndex;

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
	}

	UIEditor *UIEditor::Get()
	{
		return s_Instance;
	}

}
