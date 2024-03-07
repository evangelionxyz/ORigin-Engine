#include "SpriteSheetEditor.h"
#include "Origin\Renderer\Renderer.h"
#include "Origin\Renderer\Renderer2D.h"
#include "Origin\Asset\Serializer.h"
#include "Origin\Asset\AssetManager.h"
#include "Origin\Scene\EntityManager.h"

#include <glm\gtc\type_ptr.hpp>
#include <yaml-cpp\yaml.h>
#include <imgui.h>

namespace origin
{
	SpriteSheetEditor::SpriteSheetEditor()
		: m_ViewportSize(0.0f)
	{
		m_Camera.InitOrthographic(10.0f, 0.1f, 10.0f);
		m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 2.f));

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

	void SpriteSheetEditor::CreateNewSpriteSheet()
	{
		m_SpriteSheet = SpriteSheet::Create();
	}

	void SpriteSheetEditor::SetSelectedSpriteSheet(AssetHandle handle)
	{
		m_SpriteSheet = AssetManager::GetAsset<SpriteSheet>(handle);

		m_CurrentFilepath = Project::GetActiveAssetDirectory() / Project::GetActive()->GetEditorAssetManager()->GetFilepath(handle);

		if (Deserialize() && !m_IsOpened)
		{
			m_Camera.SetOrthoSize(m_Texture->GetWidth() / 2.0f);
			m_IsOpened = true;
		}
	}

	void SpriteSheetEditor::SetMainTexture(AssetHandle handle)
	{
		if (m_SpriteSheet)
			m_SpriteSheet->SetMainTexture(handle);
	}

	void SpriteSheetEditor::AddSprite(glm::vec2 position, glm::vec2 size, glm::vec2 min, glm::vec2 max)
	{
		SpriteData sprite {};
		sprite.Position = position;
		sprite.Size = size;
		sprite.Min = min;
		sprite.Max = max;
		m_SpriteSheet->Sprites.push_back(sprite);
	}

	void SpriteSheetEditor::RemoveSprite(int index)
	{
		m_SpriteSheet->Sprites.erase(m_SpriteSheet->Sprites.begin() + index);
	}

	void SpriteSheetEditor::OnImGuiRender()
	{
		if (m_IsOpened)
		{
			ImGui::Begin("Sprite Sheet Editor", &m_IsOpened, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			
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

			const float spriteSize = 64.0f;

			ImTextureID texture = reinterpret_cast<ImTextureID>(m_Framebuffer->GetColorAttachmentRendererID());
			ImGui::Image(texture, { m_ViewportSize.x, m_ViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));

			ImGui::End();

			ImGui::Begin("Sprite Sheet Controller");
			ImGui::Text("Mouse (%d, %d)", m_Mouse.x, m_Mouse.y);
			ImGui::Text("Selected %d", m_SelectedIndex);
			ImGui::Text("Hovered %d", m_HoveredIndex);

			if (m_Texture)
			{
				if (ImGui::Button("Save"))
				{
					Serialize(m_CurrentFilepath);
				}

				texture = reinterpret_cast<ImTextureID>(m_Texture->GetRendererID());
				ImVec2 atlasSize { (float)m_Texture->GetWidth(), (float)m_Texture->GetHeight() };

				if (ImGui::Button("Add"))
				{
					SpriteSheetController control;
					m_Controls.push_back(control);
					m_SelectedIndex = m_Controls.size() - 1;
				}

				if (!m_Controls.empty() && m_SelectedIndex >= 0)
				{
					auto &c = m_Controls[m_SelectedIndex];
					c.Min = { (c.Position.x * c.Size.x) / atlasSize.x, (c.Position.y * c.Size.y) / atlasSize.y };
					c.Max = { ((c.Position.x + 1.0) * c.Size.x) / atlasSize.x, ((c.Position.y + 1.0f) * c.Size.y) / atlasSize.y };
					ImGui::DragFloat("##xPos", &c.Position.x, c.Size.x / atlasSize.x, 0.0f, atlasSize.x / c.Size.x - 1.0f);
					ImGui::DragFloat("##yPos", &c.Position.y, c.Size.y / atlasSize.y, 0.0f, atlasSize.y / c.Size.y - 1.0f);
					ImGui::DragFloat2("Size", glm::value_ptr(c.Size), 1.0f, 0.0f);
				}

				const float padding = 10.0f;
				const float cellSize = spriteSize + padding;
				const float panelWidth = ImGui::GetWindowContentRegionWidth();
				int columnCount = static_cast<int>(panelWidth / cellSize);
				if (columnCount < 1)
					columnCount = 1;

				ImGui::Columns(columnCount, nullptr, false);

				for (int i = 0; i < m_Controls.size(); i++)
				{
					auto &control = m_Controls[i];
					ImGui::PushID(i);
					ImGui::ImageButton(texture, { spriteSize, spriteSize }, { control.Min.x, control.Max.y }, { control.Max.x, control.Min.y });

					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						m_SelectedIndex = i;

					if (ImGui::BeginPopupContextItem("sprite_context", 1))
					{
						if (ImGui::MenuItem("Delete"))
						{
							m_Controls.erase(m_Controls.begin() + i);
							if(m_SelectedIndex >= 0)
								m_SelectedIndex = i > 0 ? i - 1: 0;
						}

						ImGui::EndPopup();
					}

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text("Position: %f, %f ", control.Position.x, control.Position.y);
						ImGui::Text("Size: %f, %f ", control.Size.x, control.Size.y);
						ImGui::Text("Min: %f, %f ", control.Min.x, control.Min.y);
						ImGui::Text("Max: %f, %f ", control.Max.x, control.Max.y);
						ImGui::EndTooltip();
					}

					ImGui::NextColumn();
					ImGui::PopID();
				}
			}
			ImGui::End();
		}
	}

	void SpriteSheetEditor::OnRender(float ts)
	{
		if (!m_IsOpened)
			return;

		m_Camera.OnUpdate(ts);

		RenderCommand::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		m_Framebuffer->Bind();
		RenderCommand::Clear();
		m_Framebuffer->ClearAttachment(1, -1);

		if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification();
				m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (m_ViewportSize.x != spec.Width || m_ViewportSize.y != spec.Height))
		{
			m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_Framebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
		}

		Renderer2D::Begin(m_Camera);
		if (m_Texture)
		{
			int texX = m_Texture->GetWidth() / 2;
			int texY = m_Texture->GetHeight() / 2;
			Renderer2D::DrawQuad(glm::scale(glm::mat4(1.0f), { texX, texY, 1.0 }), m_Texture, -20);

			int i = 0;
			for (auto &c : m_Controls)
			{
				float sizeX = c.Size.x / 2.0f;
				float sizeY = c.Size.y / 2.0f;
				float posX = c.Position.x * sizeX - texX / 2.0f;
				float posY = c.Position.y * sizeY - texY / 2.0f;

				/*glm::vec4 col = i == m_SelectedIndex ? glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
				Renderer2D::DrawRect({ posX + sizeX / 2.0f, posY + sizeY / 2.0f, 1.0f },
														 { sizeX, sizeY }, col);*/
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), { posX + sizeX / 2.0f, posY + sizeY / 2.0f, 1.0f })
					* glm::scale(glm::mat4(1.0f), glm::vec3(sizeX, sizeY, 1.0f));

				Renderer2D::DrawQuad(transform, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), i);
				i++;
			}
		}


		Renderer2D::End();

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

	bool SpriteSheetEditor::Serialize(const std::filesystem::path &filepath)
	{
		m_CurrentFilepath = filepath;

		for (auto &control : m_Controls)
			AddSprite(control.Position, control.Size, control.Min, control.Max);

		YAML::Emitter out;
		out << YAML::BeginMap; // SpriteSheet
		out << YAML::Key << "SpriteSheet";
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Texture" << YAML::Value << m_SpriteSheet->GetTextureHandle();
			out << YAML::Key << "Sprites" << YAML::Value << YAML::BeginSeq;
			for (auto &s : m_SpriteSheet->Sprites)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Position" << s.Position;
				out << YAML::Key << "Size" << s.Size;
				out << YAML::Key << "Min" << s.Min;
				out << YAML::Key << "Max" << s.Max;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}
		out << YAML::EndMap; // !SpriteSheet

		std::ofstream outFile(filepath.string());
		outFile << out.c_str();

		return true;
	}

	bool SpriteSheetEditor::Deserialize()
	{
		std::ifstream stream(m_CurrentFilepath.string());

		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node spriteSheet = YAML::Load(strStream.str());
		if (!spriteSheet["SpriteSheet"])
			return false;

		if (YAML::Node data = spriteSheet["SpriteSheet"])
		{
			AssetHandle textureHandle = data["Texture"].as<uint64_t>();
			m_SpriteSheet->SetMainTexture(textureHandle);
			m_Texture = AssetManager::GetAsset<Texture2D>(textureHandle);

			if (!m_Texture)
			{
				OGN_CORE_ERROR("[SpriteSheetEditor] Main Texture Is Not Valid");
				OGN_CORE_ERROR("[SpriteSheetEditor] {}", textureHandle);
				return false;
			}

			glm::vec2 atlasSize = { (float)m_Texture->GetWidth(), (float)m_Texture->GetHeight() };

			if (YAML::Node sprites = data["Sprites"])
			{
				for (auto s : sprites)
				{
					AddSprite(
						s["Position"].as<glm::vec2>(),
						s["Size"].as<glm::vec2>(),
						s["Min"].as<glm::vec2>(),
						s["Max"].as<glm::vec2>()
					);
				}
			}


			for (auto &sprite : m_SpriteSheet->Sprites)
			{
				SpriteSheetController control;
				control.Position = sprite.Position;
				control.Size = sprite.Size;
				control.Min = sprite.Min;
				control.Max = sprite.Max;
				m_Controls.push_back(control);
			}
		}
		return true;
	}

	void SpriteSheetEditor::OnEvent(Event &e)
	{
		m_Camera.OnEvent(e);
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(SpriteSheetEditor::OnMouseButtonPressed));
	}

	bool SpriteSheetEditor::OnMouseButtonPressed(MouseButtonPressedEvent &e)
	{
		if (e.GetMouseButton() == Mouse::ButtonLeft && IsHovered)
		{
			if (m_HoveredIndex != m_SelectedIndex && m_HoveredIndex >= 0)
				m_SelectedIndex = m_HoveredIndex;
			else
				m_SelectedIndex = -1;
		}

		return false;
	}

}
