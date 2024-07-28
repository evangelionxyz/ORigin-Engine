// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
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
		void OnUpdate(Timestep ts) override;
		void OnEvent(Event& e) override;
		void SetViewportSize(float width, float height) override;
		void SetPosition(const glm::vec3 &position) override;
		void SetFocalPoint(const glm::vec3 &position) override;
		void SetPitch(float pitch) override;
		void SetYaw(float yaw) override;
        bool OnMouseScroll(MouseScrolledEvent &e);
        void MousePan(const glm::vec2 &delta);
        void MouseRotate(const glm::vec2 &delta);
        void MouseZoom(const float dela);
		void SetDistance(float distance) override;
        float RotationSpeed() const;

        std::pair<float, float> PanSpeed() const;
		void UpdateAudioListener(Timestep ts) override;
		float GetZoomSpeed() const;
		const glm::mat4 &GetProjection() const override;
		const glm::mat4 &GetViewMatrix() const override;

        float GetYaw() const override { return m_Yaw; }
        float GetPitch() const override { return m_Pitch; }
        float GetFOV() const override { return m_FOV; }
        float GetNear() const override { return m_NearClip; }
        float GetFar() const override { return m_FarClip; }
        float GetAspectRatio() const override { return m_AspectRatio; }
        float GetOrthoScale() const override { return m_OrthoScale; }
        float GetOrthoNear() const override { return m_OrthoNearClip; }
        float GetOrthoFar() const override { return m_OrthoFarClip; }
        float GetWidth() const override { return m_ViewportWidth; }
        float GetHeight() const override { return m_ViewportHeight; }
        bool IsPerspective() const override { return m_ProjectionType == ProjectionType::Perspective; }
        const ProjectionType GetProjectionType() const override { return m_ProjectionType; }
        const CameraStyle GetStyle() override { return m_CameraStyle; }
        const AspectRatioType GetAspectRatioType() const override { return m_AspecRatioType; }
        glm::vec3 GetUpDirection() const { return glm::rotate(glm::quat({ -m_Pitch, -m_Yaw, 0.0f }), { 0.0f, 1.0f, 0.0f }); }
        glm::vec3 GetRightDirection() const { return glm::rotate(glm::quat({ -m_Pitch, -m_Yaw, 0.0f }), { 1.0f, 0.0f, 0.0f }); }
        glm::vec3 GetForwardDirection() const { return glm::rotate(glm::quat({ -m_Pitch, -m_Yaw, 0.0f }), { 0.0f, 0.0f, -1.0f }); }
        Frustum &GetFrustum() override { return m_Frustum; }
	private:
        void UpdateProjection() override;
        void UpdateView() override;
	};
}