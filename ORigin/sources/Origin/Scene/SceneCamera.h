// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "ProjectionType.h"

namespace origin {

	class SceneCamera
	{
	public:
		SceneCamera();
		virtual ~SceneCamera() = default;

		enum class AspectRatioType
		{
			Free = 0,
			SixteenByTen = 1,
			SixteenByNine = 2,
			TwentyOneByNine = 3,
			FourByThree = 4
		};

		void SetViewportSize(uint32_t width, uint32_t height);
		void SetPosition(const glm::vec3& position) { m_Position = position; }

		const glm::vec2& GetViewportSize() { return m_ViewportSize; }
		const glm::mat4 GetViewProjection() const { return (m_Projection * m_View) * glm::inverse(m_View); }
		const glm::mat4& GetProjection() const;
		const glm::mat4& GetViewMatrix() const;
		const glm::vec3& GetPosition() const { return m_Position; }

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

		void SetOrthographicSize(float size) { m_OrthoSize = size; RecalculateProjection(); }
		void SetOrthographicNearClip(float nearClip) { m_OrthoNear = nearClip; RecalculateProjection(); }
		void SetOrthographicFarClip(float farClip) { m_OrthoFar = farClip; RecalculateProjection(); }

		float GetOrthographicSize() const { return m_OrthoSize; }
		float GetOrthographicNearClip() const { return m_OrthoNear; }
		float GetOrthographicFarClip() const { return m_OrthoFar; }

		float GetAspectRatioSize() const { return m_AspectRatio; }

		void SetProjectionType(ProjectionType type) { m_ProjectionType = type; }
		ProjectionType GetProjectionType() const { return m_ProjectionType;  }
		
		void SetAspectRatioType(AspectRatioType type) { m_AspecRatioType = type; }
		AspectRatioType GetAspectRatioType() const { return m_AspecRatioType; }


	private:
		AspectRatioType m_AspecRatioType = AspectRatioType::Free;
		ProjectionType m_ProjectionType = ProjectionType::Perspective;

		void RecalculateProjection();

		float m_AspectRatio = 1.778f;
		glm::mat4 m_Projection = glm::mat4(1.0f);
		glm::mat4 m_View = glm::mat4(1.0f);

		glm::vec2 m_ViewportSize = glm::vec2(0.0f);
		glm::vec3 m_Position = glm::vec3(1.0f);

		// Perspective
		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 100.0f;

		// Orthographic
		float m_OrthoSize = 10.0f;
		float m_OrthoNear = -1.0f, m_OrthoFar = 100.0f;
	};

}