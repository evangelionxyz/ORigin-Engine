// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Scene/Component/Camera.h"
#include "Origin/Utils/Time.h"
#include "Origin/IO/Events/Event.h"
#include "Origin/IO/Events/MouseEvent.h"

#include <glm/glm.hpp>

namespace Origin {

	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void SetPosition(glm::vec3 position) { m_Position = position; }
		void SetViewportSize(float width, float height);
		inline void SetDistance(float distance) { m_Distance = distance; }
		inline void SetPitch(float pitch) { m_Pitch = pitch; }
		inline void SetYaw(float yaw) { m_Yaw = yaw; }
		inline void SetFov(float fov) { m_FOV = fov; }

		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 GetViewProjection() const { return m_Projection * m_ViewMatrix; }
		inline float GetDistance() const { return m_Distance; }
		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::quat GetOrientation() const;

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
		void EnableMovement(bool enable) { m_EnableMovement = enable; }
		bool IsActive() { return m_EnableMovement; }

		float GetZoomSpeed() { return ZoomSpeed(); }

	private:
		void UpdateProjection();
		void UpdateView();

		// Event
		bool OnMouseScroll(MouseScrolledEvent& e);

		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(float delta);

		glm::vec3 CalculatePosition() const;
		std::pair<float, float> PanSpeed() const;
		float RotationSpeed() const;
		float ZoomSpeed() const;

		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;
		bool m_EnableMovement;

		glm::mat4 m_ViewMatrix;
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 0.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
	};
}