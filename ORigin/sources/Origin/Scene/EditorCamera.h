// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "ProjectionType.h"
#include "Origin\IO\Events\Event.h"
#include "Origin\IO\Events\MouseEvent.h"

#include "Origin\Audio\AudioListener.h"

#include <glm\glm.hpp>

namespace origin {

	class Entity;
	enum CameraStyle
	{
		Pivot = 0,
		FreeMove,
	};

	class EditorCamera
	{
	public:
		EditorCamera() = default;

		void InitPerspective(float fovy, float aspectRatio, float nearClip, float farClip);
		void InitOrthographic(float size, float nearClip, float farClip);

		void OnUpdate(float ts);
		void OnEvent(Event& e);

		void SetEntityObject(Entity entity);
		void SetViewportSize(float width, float height);

		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; }
		void SetPosition(const glm::vec3& position) { m_Position = position; }

		inline void SetDistance(float distance) { m_Distance = distance; }
		inline void SetPitch(float pitch) { m_Pitch = pitch; }
		inline void SetYaw(float yaw) { m_Yaw = yaw; }
		inline void SetFov(float fov) { m_FOV = fov; UpdateProjection(); }

		inline float GetDistance() const { return m_Distance; }

		const glm::vec3& GetPosition() const { return m_Position; }
		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::quat GetOrientation() const;

		const glm::mat4& GetProjection() const;
		const glm::mat4& GetViewMatrix() const;
		virtual const glm::mat4 GetViewProjection() const { return m_Projection * m_View; }

		void SetStyle(CameraStyle style) { m_CameraStyle = style; }
		const CameraStyle GetStyle() { return m_CameraStyle; }

		void EnableMovement(bool enable) { m_EnableMovement = enable; }
		bool IsActive() { return m_EnableMovement; }
		const ProjectionType GetProjectionType() const { return m_ProjectionType; }

		float GetYaw() const { return m_Yaw; }
		float GetPitch() const { return m_Pitch; }
		float GetZoomSpeed() { return ZoomSpeed(); }
		float GetFOV() const { return m_FOV; }
		float GetAspectRatio() { return m_AspectRatio; }
		float GetOrthoSize() const { return m_OrthoSize; }
		float GetWidth() const { return m_ViewportWidth; }
		float GetHeight() const { return m_ViewportHeight; }

		void UpdateAudioListener(float deltaTime);
		AudioListener GetAudioListener() { return m_AudioListener; }

	private:
		AudioListener m_AudioListener;
		CameraStyle m_CameraStyle = Pivot;
		ProjectionType m_ProjectionType = ProjectionType::Perspective;

		void UpdateProjection();
		void UpdateView();

		bool OnMouseScroll(MouseScrolledEvent& e);
		
		void MousePan(const glm::vec2& delta);
		void MouseRotate(const glm::vec2& delta);
		void MouseZoom(const float delta);

		float ZoomSpeed() const;
		float RotationSpeed() const;
		std::pair<float, float> PanSpeed() const;
		
		
		float m_DeltaTime;
		bool m_EnableMovement;

		float m_Distance = 8.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
		float m_MoveSpeed = 2.0f;

		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_LastPosition = glm::vec3(0.0f);
		glm::vec3 m_TargetPosition = glm::vec3(0.0f);
		glm::vec3 m_FocalPoint = glm::vec3(0.0f);
		glm::vec2 m_InitialMousePosition = glm::vec2(0.0f);

		glm::mat4 m_Projection = glm::mat4(1.0f);
		glm::mat4 m_View = glm::mat4(1.0f);
		float m_FOV = 45.0f;
		float m_AspectRatio = 1.778f;
		float m_NearClip = 0.1f;
		float m_FarClip = 5000.0f;

		float m_OrthoNearClip = -1.0f;
		float m_OrthoFarClip = 100.0f;
		float m_OrthoSize = 10.0f;
	};
}