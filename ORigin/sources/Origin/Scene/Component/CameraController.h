// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Camera.h"

#include "Origin\IO\Events\AppEvent.h"
#include "Origin\IO\Events\Event.h"
#include "Origin\IO\Events\MouseEvent.h"
#include "Origin\IO\Events\KeyEvent.h"

namespace Origin {

	class OrthoCameraController
	{
	public:
		OrthoCameraController(float aspectRatio = 1280.0f / 720.0f, bool rotate = false);

		void EditorMode(bool active = false) { m_EditorMode = active; }

		void OnUpdate(float time);
		void OnEvent(Event& e);

		float GetZoomLevel() const { return m_ZoomLevel; }
		void SetZoomLevel(float level) { m_ZoomLevel = level; }

		OrthoCamera& GetCamera() { return m_Camera; }
		const OrthoCamera& GetCamera() const { return m_Camera; }

		void OnResize(float width, float height);

		float GetWidth() { return m_Width * m_AspectRatio; }
		float GetHeight() { return m_Height * m_AspectRatio; }
		float GetAspectRatio() { return m_AspectRatio * m_ZoomLevel; }

	private:
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnWindowResized(WindowResizeEvent& e);

		float m_Width;
		float m_Height;

	private:
		float m_AspectRatio;
		float m_ZoomLevel = 2.0f;
		OrthoCamera m_Camera;
		bool m_EditorMode = false;
		bool m_IsRotate = false;
		glm::vec3 m_CameraPosition = glm::vec3(0.0f);
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 2.5f;
		float m_CameraRotationSpeed = 180.0f;
	};
}