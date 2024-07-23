// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Camera.h"
#include <glm/glm.hpp>

namespace origin {

	class SceneCamera : public Camera
	{
	public:
		SceneCamera() = default;
		void InitPerspective(float fovy, float aspectRatio, float nearClip, float farClip) override;
		void InitOrthographic(float size, float nearClip, float farClip) override;
		void SetViewportSize(float width, float height) override;
		void SetOrthoScale(float size) override;
		void SetAspectRatioType(AspectRatioType type) override;
		const glm::mat4 GetViewProjection() const override { return (m_Projection * m_View) * glm::inverse(m_View); }
		const glm::mat4 &GetProjection() const override;
		const glm::mat4 &GetViewMatrix() const override;
	private:
		void UpdateProjection() override;
		void UpdateView() override;
	};

}