// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "Origin/Core/Event.h"
#include "Origin/Core/MouseEvent.h"
#include "Origin/Audio/AudioListener.h"
#include "Origin/Core/Time.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace origin {

	class Entity;
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		void InitPerspective(float fovy, float aspectRatio, float nearClip, float farClip) override;
		void InitOrthographic(float size, float nearClip, float farClip) override;
        void SetProjectionType(ProjectionType type) override;
		void OnUpdate(Timestep ts, const glm::vec2 &screenMin, const glm::vec2 &screenMax) override;
		void OnEvent(Event& e) override;
        void SetOrthoScale(float value) override;
        void SetOrthoScaleMax(float max) override;
		void SetViewportSize(float width, float height) override;
        void SetFov(float fovy) override;
        void SetNear(float nearClip) override;
        void SetFar(float farClip) override;
		void SetPosition(const glm::vec3 &position);
		void SetFocalPoint(const glm::vec3 &position);
        void SetStyle(CameraStyle style);
		void SetPitch(float pitch);
		void SetYaw(float yaw);
        bool OnMouseScroll(MouseScrolledEvent &e);
        void MousePan(const glm::vec2 &delta);
        void MouseRotate(const glm::vec2 &delta, float dt);
        void MouseZoom(const float dela);
		void SetDistance(float distance);
        void SetViewMatrix(const glm::mat4 &viewMatrix) override;
        float RotationSpeed() const;

        std::pair<float, float> PanSpeed() const;
        void UpdateAudioListener(Timestep ts);
        const float GetDistance() const { return m_Distance; }
		float GetZoomSpeed() const;
        const float GetYaw() const override { return m_Yaw; }
        const float GetPitch() const override { return m_Pitch; }
        const float GetFOV() const  override{ return m_FOV; }
        const float GetNear() const override { return m_NearClip; }
        const float GetFar() const override { return m_FarClip; }
        const float GetAspectRatio() const override { return m_AspectRatio; }
        const float GetOrthoScale() const override { return m_OrthoScale; }
        const float GetOrthoNear() const override { return m_OrthoNearClip; }
        const float GetOrthoFar() const override { return m_OrthoFarClip; }
        const bool IsPerspective() const override { return m_ProjectionType == ProjectionType::Perspective; }
		const glm::mat4 GetProjectionMatrix() const override;
		const glm::mat4 GetViewMatrix() const override;
        const glm::mat4 GetViewProjection() const override;
        const glm::vec3 GetPosition() const override;
        const glm::vec2 GetViewportSize() const override;
        const glm::vec2 GetOrthoSize() const override { return m_OrthoSize; }
        glm::vec3 GetUpDirection() const { return glm::rotate(glm::quat({ -m_Pitch, -m_Yaw, 0.0f }), { 0.0f, 1.0f, 0.0f }); }
        glm::vec3 GetRightDirection() const { return glm::rotate(glm::quat({ -m_Pitch, -m_Yaw, 0.0f }), { 1.0f, 0.0f, 0.0f }); }
        glm::vec3 GetForwardDirection() const { return glm::rotate(glm::quat({ -m_Pitch, -m_Yaw, 0.0f }), { 0.0f, 0.0f, -1.0f }); }
        const CameraStyle GetStyle() { return m_CameraStyle; }
        const ProjectionType GetProjectionType() const override { return m_ProjectionType; }

        void SetAllowedMove(bool active);

	private:
        void UpdateProjection() override;
        void UpdateView() override;
        CameraStyle m_CameraStyle = FreeMove;
        float m_Distance = 8.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;
        bool m_IsInViewport = false;
        bool m_AllowedMove = false;
        glm::vec3 m_Velocity = glm::vec3(0.0f);
        glm::vec2 m_LastMousePos = glm::vec2(0.0f);

        const float ACCELERATION = 70.0f;
        const float DECELERATION = 130.0f;
        const float MAX_SPEED = 100.0f;

        glm::vec2 m_AngularVelocity = glm::vec2(0.0f);
        const float ROTATION_SPEED = 0.7f;
        const float DAMPING = 0.9f;
        const float SENSITIVITY = 0.1f;
	};
}

#endif