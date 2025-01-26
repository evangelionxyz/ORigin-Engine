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

	glm::mat4 GetViewMatrix() const override;
	glm::mat4 GetProjectionMatrix() const override;
	glm::mat4 GetViewProjection() const override;
	glm::vec2 GetOrthoSize() const override;
	glm::vec2 GetViewportSize() const override;
	bool IsPerspective() const override;
	float GetAspectRatio() const override;
	float GetFOV() const override { return m_FOV; }
	float GetNear() const override { return m_NearClip; }
	float GetFar() const override { return m_FarClip; }
	float GetOrthoScale() const override { return m_OrthoScale; }
	float GetOrthoNear() const override { return m_OrthoNearClip; }
	float GetOrthoFar() const override { return m_OrthoFarClip; }

	ProjectionType GetProjectionType() const override { return m_ProjectionType; }
	AspectRatioType GetAspectRatioType() const { return m_AspectRatioType; }

private:
	void UpdateProjection() override;
	void UpdateView() override;
	glm::mat4 m_Transform = glm::mat4(1.0f);
};
}

#endif