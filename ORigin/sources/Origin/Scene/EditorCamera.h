// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "ProjectionType.h"
#include "Origin\Core\Event.h"
#include "Origin\Core\MouseEvent.h"
#include "Origin\Audio\AudioListener.h"
#include "Origin\Core\Time.h"
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
		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);
		void SetViewportSize(float width, float height);
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; }
		void SetPosition(const glm::vec3& position) { m_Position = position; }
		inline void SetDistance(float distance) { m_Distance = distance; }
		inline void SetPitch(float pitch) { m_Pitch = pitch; }
		inline void SetYaw(float yaw) { m_Yaw = yaw; }
		inline void SetFov(float fov) { m_FOV = fov; UpdateProjection(); }
		inline float GetDistance() const { return m_Distance; }
		const glm::vec3 &GetFocalPoint() const { return m_FocalPoint; }
		const glm::vec3 &GetPosition() const { return m_Position; }
		const glm::vec2 GetViewportSize() const { return { m_ViewportWidth, m_ViewportHeight }; }
		const glm::mat4 &GetProjection() const;
		const glm::mat4 &GetViewMatrix() const;
		const glm::mat4 GetViewProjection() const { return m_Projection * m_View; }
		void SetStyle(CameraStyle style) { m_CameraStyle = style; }
		void SetDraggingActive(bool enable) { m_CanDragging = enable; }
		void SetScrollingActive(bool enable) { m_CanScrolling = enable; }
		void SetMoveActive(bool enable) { m_CanMoving = enable; }
		void SetOrthoScale(float size) { m_OrthoScale = size; }
		void SetOrthoScaleMax(float size) { m_MaxOrthoScale = size; }

		float GetYaw() const { return m_Yaw; }
		float GetPitch() const { return m_Pitch; }
		float GetZoomSpeed() { return ZoomSpeed(); }
		float GetFOV() const { return m_FOV; }
		float GetNear() const { return m_NearClip; }
		float GetFar() const { return m_FarClip; }
		float GetAspectRatio() const { return m_AspectRatio; }
		float GetOrthoScale() const { return m_OrthoScale; }
		float GetWidth() const { return m_ViewportWidth; }
		float GetHeight() const { return m_ViewportHeight; }
		void UpdateAudioListener(Timestep ts);
		glm::vec3 GetUpDirection() const;
		glm::vec3 GetRightDirection() const;
		glm::vec3 GetForwardDirection() const;
		glm::quat GetOrientation() const;
		const CameraStyle GetStyle() { return m_CameraStyle; }
		const ProjectionType GetProjectionType() const { return m_ProjectionType; }
		bool IsPerspective() const { return m_ProjectionType == ProjectionType::Perspective; }

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
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_FocalPoint = glm::vec3(0.0f);
		glm::vec2 m_InitialMousePosition = glm::vec2(0.0f);
		glm::mat4 m_Projection = glm::mat4(1.0f);
		glm::mat4 m_View = glm::mat4(1.0f);
		bool m_CanDragging = true;
		bool m_CanScrolling = true;
		bool m_CanMoving = true;
		float m_Distance = 8.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;
		float m_ViewportWidth = 1280, m_ViewportHeight = 720;
		float m_MoveSpeed = 2.0f;
		float m_FOV = 45.0f;
		float m_AspectRatio = 1.778f;
		float m_NearClip = 0.1f;
		float m_FarClip = 5000.0f;
		float m_OrthoNearClip = -1.0f;
		float m_OrthoFarClip = 100.0f;
		float m_OrthoScale = 10.0f;
		float m_MaxOrthoScale = 100.0f;
	};
}