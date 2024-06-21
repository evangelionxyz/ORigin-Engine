// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\Event.h"
#include "Origin\Scene\Entity.h"
#include "Origin\Core\KeyEvent.h"
#include "Origin\Scene\EditorCamera.h"
#include "Origin\Scene\Components.h"
#include "Origin\Scene\Scene.h"

namespace origin
{
	class UIEditor
	{
	public:
		UIEditor(Scene *scene);
		void SetContext(Scene *scene);

		void SetActive(UIComponent *component);
		void OnCamViewportSizeChange();
		void CreateNewText();
		void CreateNewTexture();

		void OnUpdate(Timestep ts);
		void OnImGuiRender();

		void OnEvent(Event &e);
		bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
		bool OnKeyPressed(KeyPressedEvent &e);
		void OnMouse(float ts);

		void Open();

		static UIEditor *Get();

		bool IsFocused = false;
		bool IsHovered = false;
		bool IsOpened = false;

	private:
		Scene *m_Scene = nullptr;
		EditorCamera m_Camera;
		UIComponent *m_Component = nullptr;

		std::shared_ptr<Framebuffer> m_Framebuffer;

		glm::vec2 m_EditorViewportSize = { 1.0f, 1.0f };
		glm::vec2 m_EditorViewportBounds[2] = { {0.0f, 0.0f}, {0.0f, 0.0f} };
		glm::ivec2 m_Mouse = { 0, 0 };
		glm::vec2 m_MoveTranslation = { 0.0f, 0.0f };
		glm::ivec2 m_SceneCamViewportSize = { 0, 0 };

		int m_SelectedIndex = 0;
		int m_HoveredIndex = -1;
	};

}
