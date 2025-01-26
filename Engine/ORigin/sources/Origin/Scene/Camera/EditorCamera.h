// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef EDITOR_CAMERA_H
#define EDITOR_CAMERA_H

#include "Origin/Core/Event.h"
#include "Origin/Core/MouseEvent.h"
#include "Origin/Audio/AudioListener.h"
#include "Origin/Core/Time.h"
#include "Origin/Core/KeyCodes.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace origin {

	class Entity;
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		
		void InitPerspective(f32 fovy, f32 aspect_ratio, f32 near_clip, f32 far_clip) override;
		void InitOrthographic(f32 size, f32 near_clip, f32 far_clip) override;
        void SetProjectionType(ProjectionType type) override;
		void OnUpdate(Timestep ts) override;
        void SetOrthoScale(f32 value) override;
        void SetOrthoScaleMax(f32 max) override;
		void SetViewportSize(u32 width, u32 height) override;
        void SetFov(f32 fovy) override;
        void SetNear(f32 near_clip) override;
        void SetFar(f32 far_clip) override;
		void SetPosition(const glm::vec3 &position);
		void SetFocalPoint(const glm::vec3 &position);
        void SetStyle(CameraStyle style);
		void SetPitch(f32 pitch);
		void SetYaw(f32 yaw);
        void MousePan(const glm::vec2 &delta);
        void MouseRotate(const glm::vec2 &delta);
        void MouseZoom(f32 delta);
		void SetDistance(f32 distance);
        void SetViewMatrix(const glm::mat4 &view_matrix) override;
        void OnMouseScroll(f32 delta);
        void OnMouseMove(const glm::vec2 &delta);
        static f32 RotationSpeed();

        std::pair<f32, f32> PanSpeed() const;
        void UpdateAudioListener(Timestep ts);
		f32 GetDistance() const { return m_distance; }
		f32 GetZoomSpeed() const;
        f32 GetYaw() const override { return m_yaw; }
        f32 GetPitch() const override { return m_pitch; }
        f32 GetFOV() const  override{ return m_FOV; }
        f32 GetNear() const override { return m_NearClip; }
        f32 GetFar() const override { return m_FarClip; }
        f32 GetAspectRatio() const override { return m_AspectRatio; }
        f32 GetOrthoScale() const override { return m_OrthoScale; }
        f32 GetOrthoNear() const override { return m_OrthoNearClip; }
        f32 GetOrthoFar() const override { return m_OrthoFarClip; }
		glm::mat4 GetProjectionMatrix() const override;
		glm::mat4 GetViewMatrix() const override;
        glm::mat4 GetViewProjection() const override;
        glm::vec3 GetPosition() const override;
        glm::vec2 GetViewportSize() const override;
        glm::vec2 GetOrthoSize() const override { return m_OrthoSize; }
        glm::vec3 GetUpDirection() const { return rotate(glm::quat({ -m_pitch, -m_yaw, 0.0f }), { 0.0f, 1.0f, 0.0f }); }
        glm::vec3 GetRightDirection() const { return rotate(glm::quat({ -m_pitch, -m_yaw, 0.0f }), { 1.0f, 0.0f, 0.0f }); }
        glm::vec3 GetForwardDirection() const { return rotate(glm::quat({ -m_pitch, -m_yaw, 0.0f }), { 0.0f, 0.0f, -1.0f }); }
        CameraStyle GetStyle() const { return m_camera_style; }
        ProjectionType GetProjectionType() const override { return m_ProjectionType; }
        bool IsPerspective() const override { return m_ProjectionType == ProjectionType::Perspective; }

        void UpdateProjection() override;
        void UpdateView() override;

	private:
        CameraStyle m_camera_style = FreeMove;
        f32 m_distance = 8.0f;
        f32 m_pitch = 0.0f;
		f32 m_yaw = 0.0f;
		
        glm::vec3 m_velocity = glm::vec3(0.0f);
        glm::vec2 m_last_mouse_pos = glm::vec2(0.0f);
        glm::vec3 m_last_switch_position = glm::vec3(0.0f);

        const f32 ACCELERATION = 70.0f;
        const f32 DECELERATION = 130.0f;
        const f32 MAX_SPEED = 100.0f;
	};
}

#endif