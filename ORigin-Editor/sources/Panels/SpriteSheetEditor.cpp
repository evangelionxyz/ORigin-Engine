#include "SpriteSheetEditor.h"
#include "Origin\Renderer\Renderer.h"
#include "Origin\Renderer\Renderer2D.h"
#include "Origin\Asset\Serializer.h"
#include "Origin\Asset\AssetManager.h"

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
		std::filesystem::path path = Project::GetActiveAssetDirectory() / Project::GetActive()->GetEditorAssetManager()->GetFilepath(handle);
		Deserialize(path);
	}

	void SpriteSheetEditor::AddTexture(AssetHandle handle)
	{
		if (m_SpriteSheet)
			m_SpriteSheet->AddTexture(handle);
	}

	void SpriteSheetEditor::AddSprite(int position, glm::vec2 min, glm::vec2 max)
	{
		SpriteData sprite;
		sprite.Min = min;
		sprite.Max = max;
		sprite.Position = position;
		m_SpriteSheet->Sprites.push_back(sprite);
	}

	void SpriteSheetEditor::RemoveSprite(int index)
	{
		m_SpriteSheet->Sprites.erase(m_SpriteSheet->Sprites.begin() + index);
	}

	void SpriteSheetEditor::OnImGuiRender()
	{
		ImGui::Begin("Sprite Sheet Editor", nullptr, ImGuiWindowFlags_NoScrollbar);

		const float spriteSize = 64.0f;
		ImGui::BeginChild("sprite_sheet_editor_vp", { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - spriteSize * 2.5f },
											ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiChildFlags_ResizeY);

		IsFocused = ImGui::IsWindowFocused();
		IsHovered = ImGui::IsWindowHovered();

		if (ImGui::Button("Save"))
		{
			if (!m_Filepath.empty())
				Serialize(m_Filepath);
		}

		m_ViewportSize = glm::vec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		ImTextureID texture = reinterpret_cast<ImTextureID>(m_Framebuffer->GetColorAttachmentRendererID());
		ImGui::Image(texture, { m_ViewportSize.x, m_ViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndChild();

		ImGui::BeginChild("sprites", { ImGui::GetContentRegionAvail().x, spriteSize * 2.4f});
		if (m_Texture)
		{
			texture = reinterpret_cast<ImTextureID>(m_Texture->GetRendererID());
			ImVec2 atlasSize { (float)m_Texture->GetWidth(), (float)m_Texture->GetHeight() };
			{
				static glm::ivec2 spritePositions = glm::ivec2(0, 0);
				static glm::ivec2 size = glm::ivec2(16, 16);
				glm::vec2 min { (spritePositions.x * size.x) / atlasSize.x, ((spritePositions.y - 1.0f) * size.y) / atlasSize.y };
				glm::vec2 max { ((spritePositions.x + 1.0) * size.x) / atlasSize.x, (spritePositions.y * size.y) / atlasSize.y };
				ImGui::Columns(2);
				ImGui::Image(texture, { spriteSize, spriteSize }, ImVec2(min.x, min.y), ImVec2(max.x, max.y));
				ImGui::NextColumn();
				ImGui::DragInt2("Positions", glm::value_ptr(spritePositions), 1.0f);
				ImGui::DragInt2("Size", glm::value_ptr(size), 1.0f);

				if (ImGui::Button("Add"))
					AddSprite(m_SpriteSheet->Sprites.size(), min, max);
			}

			const float padding = 10.0f;
			const float cellSize = spriteSize + padding;
			const float panelWidth = ImGui::GetWindowContentRegionWidth();
			int columnCount = static_cast<int>(panelWidth / cellSize);
			if (columnCount < 1)
				columnCount = 1;

			ImGui::Columns(columnCount, nullptr, false);

			for (int i = 0; i < m_SpriteSheet->Sprites.size(); i++ )
			{
				auto &s = m_SpriteSheet->Sprites[i];
				ImGui::PushID(i);
				ImGui::ImageButton(texture, { spriteSize, spriteSize }, { s.Min.x, s.Min.y }, { s.Max.x, s.Max.y });
				if (ImGui::BeginPopupContextItem("sprite_context", 1))
				{
					if (ImGui::MenuItem("Delete"))
						RemoveSprite(i);
						
					ImGui::EndPopup();
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Position: %d, ", s.Position);
					ImGui::Text("Min: %f, %f, ", s.Min.x, s.Min.y);
					ImGui::Text("Max: %f, %f, ", s.Max.x, s.Max.y);
					ImGui::EndTooltip();
				}

				ImGui::NextColumn();
				ImGui::PopID();
			}
		}
		ImGui::EndChild();

		ImGui::End();
	}

	void SpriteSheetEditor::OnRender(float ts)
	{
		m_Camera.EnableMovement(IsFocused && IsHovered);
		m_Camera.OnUpdate(ts);

		RenderCommand::ClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		m_Framebuffer->Bind();
		RenderCommand::Clear();

		if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification();
				m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && (m_ViewportSize.x != spec.Width || m_ViewportSize.y != spec.Height))
		{
			m_Camera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
			m_Framebuffer->Resize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
		}

		Renderer2D::Begin(m_Camera);
		if (m_Texture)
		{
			float x = 10.0f;
			float y = x * ((float)m_Texture->GetHeight() / (float)m_Texture->GetWidth());

			Renderer2D::DrawQuad(glm::scale(glm::mat4(1.0f), glm::vec3(x, y, 1.0f)), m_Texture);
		}
			
		Renderer2D::End();

		m_Framebuffer->Unbind();
	}

	bool SpriteSheetEditor::Serialize(const std::filesystem::path &filepath)
	{
		m_Filepath = filepath;

		YAML::Emitter out;
		out << YAML::BeginMap; // SpriteSheet
		out << YAML::Key << "SpriteSheet";
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Texture" << YAML::Value << m_SpriteSheet->GetTextureHandle();
			out << YAML::Key << "Sprites" << YAML::Value << YAML::BeginSeq;
			for (auto &s : m_SpriteSheet->Sprites)
			{
				out << YAML::Key << "Position" << s.Position;
				out << YAML::Key << "Min" << s.Min;
				out << YAML::Key << "Max" << s.Max;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}
		out << YAML::EndMap; // !SpriteSheet

		std::ofstream outFile(filepath.string());
		outFile << out.c_str();

		return true;
	}

	bool SpriteSheetEditor::Deserialize(const std::filesystem::path &filepath)
	{
		std::ifstream stream(filepath.string());
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node spriteSheet = YAML::Load(strStream.str());
		if (!spriteSheet["SpriteSheet"])
			return false;

		if (YAML::Node data = spriteSheet["SpriteSheet"])
		{
			AssetHandle textureHandle = data["Texture"].as<uint64_t>();
			m_Texture = AssetManager::GetAsset<Texture2D>(textureHandle);

			if (YAML::Node sprites = data["Sprites"])
			{
				for (auto s : sprites)
				{
					AddSprite(
						s["Position"].as<int>(),
						s["Min"].as<glm::vec2>(),
						s["Max"].as<glm::vec2>()
					);
				}
			}

		}
		return true;
	}

	void SpriteSheetEditor::OnEvent(Event &e)
	{
		m_Camera.OnEvent(e);
	}

}
