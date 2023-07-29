// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin\Scene\Camera.h"
#include "Origin\IO\Events\Event.h"
#include "Origin\IO\Events\MouseEvent.h"

#include "Origin\Audio\AudioListener.h"

#include <glm\glm.hpp>

namespace origin {

	class Entity;
	enum CameraStyle
	{
		Pivot = 0,
		FreeMove = 1
	};
	
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void OnUpdate(float ts);
		void OnEvent(Event& e);

		void SetEntityObject(Entity entity);

		void SetPosition(const glm::vec3& position) { m_Position = position; }
		void SetViewportSize(float width, float height);
		inline void SetDistance(float distance) { m_Distance = distance; }
		inline void SetPitch(float pitch) { m_Pitch = pitch; }
		inline void SetYaw(float yaw) { m_Yaw = yaw; }
		inline void SetFov(float fov) { m_FOV = fov; UpdateProjection(); }

		const glm::mat4& GetViewMatrix() const { return m_View; }
		const glm::mat4 GetViewProjection() const { return m_Projection * m_View; }
		inline float GetDistance() const { return m_Distance; }
		const glm::vec3& GetPosition() const { return m_Position; }

		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::quat GetOrientation() const;

		void SetStyle(CameraStyle style) { m_CameraStyle = style; }
		CameraStyle& GetStyle() { return m_CameraStyle; }

		float GetPitch() const { return m_Pitch; }
		float GetYaw() const { return m_Yaw; }
		void EnableMovement(bool enable) { m_EnableMovement = enable; }
		bool IsActive() { return m_EnableMovement; }

		float GetZoomSpeed() { return ZoomSpeed(); }
		float GetFOV() { return m_FOV; }

		void UpdateAudioListener();

	private:
		AudioListener m_AudioListener;
		CameraStyle m_CameraStyle = Pivot;

		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);
		
		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(const float delta);

		float ZoomSpeed() const;
		float RotationSpeed() const;
		std::pair<float, float> PanSpeed() const;
		
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;
		float m_DeltaTime;
		bool m_EnableMovement;

		float m_Distance = 8.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
		float m_MoveSpeed = 10.0f;

		glm::vec3 m_TargetPosition = glm::vec3(0.0f);
		glm::vec3 m_Position = glm::vec3(0.0f, 1.0f, 10.0f);
		glm::vec3 m_FocalPoint = glm::vec3(0.0f);
		glm::vec2 m_InitialMousePosition = glm::vec2(0.0f);
	};
}