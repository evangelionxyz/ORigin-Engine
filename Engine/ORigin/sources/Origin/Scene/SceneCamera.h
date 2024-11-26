// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include "Camera.h"
#include <glm/glm.hpp>

namespace origin {

	class SceneCamera : public Camera
	{
	public:
		SceneCamera() = default;
		void InitPerspective(float fovy, float aspectRatio, float nearClip, float farClip) override;
		void InitOrthographic(float size, float nearClip, float farClip) override;
		void SetViewportSize(u32 width, u32 height) override;
		void SetFov(float fovy) override;
		void SetNear(float nearClip) override;
		void SetFar(float farClip) override;
		void SetOrthoScale(float size) override;
		void SetOrthoNear(float nearClip) override;
		void SetOrthoFar(float farClip) override;
		void SetOrthoScaleMax(float max) override;
		void SetProjectionType(ProjectionType type) override;
		void SetAspectRatioType(AspectRatioType type);
		void SetTransform(const glm::mat4 &transform);

		const glm::mat4 GetViewMatrix() const override;
		const glm::mat4 GetProjectionMatrix() const override;
		const glm::mat4 GetViewProjection() const override;
		const glm::vec2 GetOrthoSize() const override;
		const glm::vec2 GetViewportSize() const override;
		const bool IsPerspective() const override;
		const float GetAspectRatio() const override;
		const float GetFOV() const { return m_FOV; }
		const float GetNear() const { return m_NearClip; }
		const float GetFar() const { return m_FarClip; }
		const float GetOrthoScale() const { return m_OrthoScale; }
		const float GetOrthoNear() const { return m_OrthoNearClip; }
		const float GetOrthoFar() const { return m_OrthoFarClip; }

		const ProjectionType GetProjectionType() const { return m_ProjectionType; }
		const AspectRatioType GetAspectRatioType() const { return m_AspectRatioType; }

	private:
		void UpdateProjection() override;
		void UpdateView() override;
		glm::mat4 m_Transform = glm::mat4(1.0f);
	};

}

#endif