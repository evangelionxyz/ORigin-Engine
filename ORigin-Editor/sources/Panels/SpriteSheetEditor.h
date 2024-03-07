#pragma once
#include "Origin\Scene\SpriteSheet.h"
#include "Origin\Scene\Entity.h"
#include "Origin\Renderer\Texture.h"
#include "Origin\Renderer\Framebuffer.h"
#include "Origin\Scene\EditorCamera.h"
#include "Origin\Core\UUID.h"

#include <glm\glm.hpp>

namespace origin
{

	enum ControllerCorner
	{
		NONE = -1, TOP_LEFT, BOTTOM_LEFT, TOP_RIGHT, BOTTOM_RIGHT
	};

	struct SpriteSheetController
	{
		glm::vec2 Position { 0.0f, 0.0f };
		glm::vec2 Size { 16.0f, 16.0f };
		glm::vec2 Min { 1.0f, 1.0f };
		glm::vec2 Max { 1.0f, 1.0f };

		ControllerCorner SelectedCorner;
	};

	class SpriteSheetEditor
	{
	public:
		SpriteSheetEditor();

		void CreateNewSpriteSheet();
		void SetSelectedSpriteSheet(AssetHandle handle);
		void SetMainTexture(AssetHandle handle);

		void AddSprite(glm::vec2 position, glm::vec2 size, glm::vec2 min, glm::vec2 max);;
		void RemoveSprite(int index);

		void OnImGuiRender();
		void OnRender(float ts);

		bool Serialize(const std::filesystem::path &filepath);
		bool Deserialize();

		void OnEvent(Event &e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent &e);

		bool IsFocused = false;
		bool IsHovered = false;
	private:

		void Reset();

		EditorCamera m_Camera;
		std::shared_ptr<SpriteSheet> m_SpriteSheet;
		std::shared_ptr<Framebuffer> m_Framebuffer;
		std::shared_ptr<Texture2D> m_Texture;
		std::vector<SpriteSheetController> m_Controls;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
		glm::vec2 m_ViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::ivec2 m_Mouse = { 0, 0 };
		std::filesystem::path m_CurrentFilepath;

		bool m_IsOpened = false;
		int m_SelectedIndex = 0;
		int m_HoveredIndex = -1;
	};

}

