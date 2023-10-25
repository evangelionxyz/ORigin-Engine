// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Camera.h"

namespace origin {

	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0, Orthographic = 1
		};

		SceneCamera();
		virtual ~SceneCamera() = default;
		void SetViewportSize(uint32_t width, uint32_t height);

		const glm::vec2& GetViewportSize() { return m_ViewportSize; }

		// Perspective
		void SetPerspective(float verticalFov, float nearClip, float farClip);

		void SetPerspectiveFov(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); }
		void SetPerspectiveNearClip(float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
		void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }

		float GetPerspectiveFov() const { return m_PerspectiveFOV; }
		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }


		// Orthographic
		void SetOrthographic(float size, float nearClip, float farClip);

		void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
		void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
		void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }

		float GetOrthographicSize() const { return m_OrthographicSize; }
		float GetOrthographicNearClip() const { return m_OrthographicNear; }
		float GetOrthographicFarClip() const { return m_OrthographicFar; }

		// Projection Type
		ProjectionType GetProjectionType() const { return m_ProjectionType;  }
		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; }

	private:
		void RecalculateProjection();

	private:
		ProjectionType m_ProjectionType = ProjectionType::Perspective;
		float m_AspectRatio = 1.778f;
		glm::vec2 m_ViewportSize = glm::vec2(0.0f);

		glm::vec3 m_Position = glm::vec3(1.0f);

		// Persepective
		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 100.0f;

		// Orthographic
		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 100.0f;
	};

}