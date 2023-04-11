// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "CameraController.h"

#include "Origin\IO\Input.h"
#include "Origin\IO\KeyCodes.h"
#include "Origin\IO\MouseCodes.h"

#include <algorithm>

namespace Origin
{
	OrthoCameraController::OrthoCameraController(float aspectRatio, bool rotate)
		: m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel,
			m_AspectRatio * m_ZoomLevel, -m_ZoomLevel , m_ZoomLevel), m_IsRotate(rotate)
	{
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	void OrthoCameraController::OnUpdate(float time)
	{
		float moveSpeed = 1.0f;

		if (m_EditorMode) {
			if (Input::IsKeyPressed(Key::A) && Input::IsMouseButtonPressed(Mouse::ButtonRight))
				m_CameraPosition.x -= m_CameraTranslationSpeed * time;
			else if (Input::IsKeyPressed(Key::D) && Input::IsMouseButtonPressed(Mouse::ButtonRight))
				m_CameraPosition.x += m_CameraTranslationSpeed * time;
			if (Input::IsKeyPressed(Key::S) && Input::IsMouseButtonPressed(Mouse::ButtonRight))
				m_CameraPosition.y -= m_CameraTranslationSpeed * time;
			else if (Input::IsKeyPressed(Key::W) && Input::IsMouseButtonPressed(Mouse::ButtonRight))
				m_CameraPosition.y += m_CameraTranslationSpeed * time;

			if (!m_IsRotate) {
				if (Input::IsKeyPressed(Key::LeftShift))
					moveSpeed = 3.0f;
			}

			if (m_IsRotate) {
				if (Input::IsKeyPressed(Key::LeftShift) && Input::IsKeyPressed(Key::A))
					m_CameraRotation -= m_CameraRotationSpeed * time;

				if (Input::IsKeyPressed(Key::LeftShift) && Input::IsKeyPressed(Key::D))
					m_CameraRotation += m_CameraRotationSpeed * time;
			}
		}
		else {
			if (Input::IsKeyPressed(Key::A))
				m_CameraPosition.x -= m_CameraTranslationSpeed * time;
			else if (Input::IsKeyPressed(Key::D))
				m_CameraPosition.x += m_CameraTranslationSpeed * time;
			if (Input::IsKeyPressed(Key::S))
				m_CameraPosition.y -= m_CameraTranslationSpeed * time;
			else if (Input::IsKeyPressed(Key::W))
				m_CameraPosition.y += m_CameraTranslationSpeed * time;

			if (!m_IsRotate) {
				if (Input::IsKeyPressed(Key::LeftShift))
					moveSpeed = 3.0f;
			}

			if (m_IsRotate) {
				if (Input::IsKeyPressed(Key::LeftShift) && Input::IsKeyPressed(Key::A))
					m_CameraRotation -= m_CameraRotationSpeed * time;

				if (Input::IsKeyPressed(Key::LeftShift) && Input::IsKeyPressed(Key::D))
					m_CameraRotation += m_CameraRotationSpeed * time;
			}
		}

		// reset position
		if (Input::IsKeyPressed(Key::LeftControl) && Input::IsKeyPressed(Key::R)) {
			m_CameraPosition = glm::vec3(0);
			m_CameraRotation = 0.0f;
		}

		m_Camera.SetPosition(m_CameraPosition);
		m_Camera.SetRotation(m_CameraRotation);

		m_CameraTranslationSpeed = m_ZoomLevel * moveSpeed;
	}

	void OrthoCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispathcer(e);
		dispathcer.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(OrthoCameraController::OnMouseScrolled));
		dispathcer.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(OrthoCameraController::OnWindowResized));
	}

	void OrthoCameraController::OnResize(float width, float height)
	{
		m_Width = width;
		m_Height = height;

		m_AspectRatio = width / height;
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
	}

	bool OrthoCameraController::OnMouseScrolled(MouseScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthoCameraController::OnWindowResized(WindowResizeEvent& e)
	{
		OnResize((float)e.GetWidth(), (float)e.GetHeight());
		return false;
	}
}