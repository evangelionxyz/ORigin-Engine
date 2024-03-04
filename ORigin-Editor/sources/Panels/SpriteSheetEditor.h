#pragma once
#include "Origin\Scene\SpriteSheet.h"
#include "Origin\Renderer\Texture.h"
#include "Origin\Renderer\Framebuffer.h"
#include "Origin\Scene\EditorCamera.h"
#include "Origin\Core\UUID.h"

#include <glm\glm.hpp>

namespace origin
{
	class SpriteSheetEditor
	{
	public:
		SpriteSheetEditor();
		
		void CreateNewSpriteSheet();
		void SetSelectedSpriteSheet(AssetHandle handle);
		void AddTexture(AssetHandle handle);

		void AddSprite(int position, glm::vec2 min, glm::vec2 max);;
		void RemoveSprite(int index);

		void OnImGuiRender();
		void OnRender(float ts);

		bool Serialize(const std::filesystem::path &filepath);
		bool Deserialize(const std::filesystem::path &filepath);

		void OnEvent(Event &e);

		bool IsFocused = false;
		bool IsHovered = false;
	private:
		EditorCamera m_Camera;
		std::shared_ptr<SpriteSheet> m_SpriteSheet;
		std::shared_ptr<Framebuffer> m_Framebuffer;
		std::shared_ptr<Texture2D> m_Texture;
		glm::vec2 m_ViewportSize;
		std::filesystem::path m_Filepath;
	};

}

