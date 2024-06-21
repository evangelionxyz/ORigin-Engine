// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "SceneCamera.h"
#include <glm\gtc\matrix_transform.hpp>

namespace origin
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	const glm::mat4 &SceneCamera::GetProjection() const
	{
		return m_Projection;
	}

	const glm::mat4 &SceneCamera::GetViewMatrix() const
	{
		return m_View;
	}

	void SceneCamera::SetPerspective(float verticalFov, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFov;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float scale, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthoScale = scale;
		m_OrthoNear = nearClip;
		m_OrthoFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		float aspectWidth = 0.0f, aspectHeight = 0.0f;

		switch (m_AspecRatioType)
		{
		case AspectRatioType::SixteenByNine:
			aspectWidth = 16.0f;
			aspectHeight = 9.0f;
			break;
		case AspectRatioType::SixteenByTen:
			aspectWidth = 16.0f;
			aspectHeight = 10.0f;
			break;
		case AspectRatioType::FourByThree:
			aspectWidth = 4.0f;
			aspectHeight = 3.0f;
			break;
		case AspectRatioType::TwentyOneByNine:
			aspectWidth = 21.0f;
			aspectHeight = 9.0f;
			break;
		case AspectRatioType::Free:
			aspectWidth = static_cast<float>(width);
			aspectHeight = static_cast<float>(height);
			break;
		}

		float aspectRatio = aspectWidth / aspectHeight;
		float currentAspectRatio = static_cast<float>(width) / static_cast<float>(height);

		if (currentAspectRatio > aspectRatio)
		{
			m_ViewportSize.y = static_cast<float>(height);
			m_ViewportSize.x = m_ViewportSize.y * aspectRatio;
		}
		else
		{
			m_ViewportSize.x = static_cast<float>(width);
			m_ViewportSize.y = m_ViewportSize.x * aspectRatio;
		}

		m_AspectRatio = aspectRatio;

		m_OrthoSize.x = m_OrthoScale * m_AspectRatio;
		m_OrthoSize.y = m_OrthoSize.x / m_AspectRatio;
	}

	void SceneCamera::SetAspectRatioType(AspectRatioType type)
	{
		m_AspecRatioType = type;
		SetViewportSize(static_cast<uint32_t>(m_ViewportSize.x), static_cast<uint32_t>(m_ViewportSize.y));
	}

	void SceneCamera::RecalculateProjection()
	{
		switch (m_ProjectionType)
		{
			case ProjectionType::Perspective:
				m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
				break;
			case ProjectionType::Orthographic:
				float OrthoLeft = -m_OrthoScale * m_AspectRatio / 2.0f;
				float OrthoRight = m_OrthoScale * m_AspectRatio / 2.0f;
				float OrthoTop = -m_OrthoScale / 2.0f;
				float OrthoBottom = m_OrthoScale / 2.0f;

				m_Projection = glm::ortho(OrthoLeft, OrthoRight, OrthoTop, OrthoBottom, m_OrthoNear, m_OrthoFar);
				break;
		}

		m_View = glm::translate(glm::mat4(1.0f), m_Position);
		m_View = glm::inverse(m_View);
	}
}
