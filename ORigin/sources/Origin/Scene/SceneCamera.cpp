// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "SceneCamera.h"
#include <glm\gtc\matrix_transform.hpp>

namespace origin
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	const glm::mat4& SceneCamera::GetProjection() const
	{
		return m_Projection;
	}

	const glm::mat4& SceneCamera::GetViewMatrix() const
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

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthoSize = size;
		m_OrthoNear = nearClip;
		m_OrthoFar = farClip;

		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		if (m_AspecRatioType == AspectRatioType::Free)
		{
			m_ViewportSize.x = static_cast<float>(width);
			m_ViewportSize.y = static_cast<float>(height);
		}

		else if(m_AspecRatioType == AspectRatioType::SixteenByNine)
		{
			// width = 1280
			// height = width / 16 * 9

			m_ViewportSize.x = static_cast<float>(width);
			m_ViewportSize.y = static_cast<float>(width / 16.0f * 9.0f);
		}

		m_AspectRatio = m_ViewportSize.x / m_ViewportSize.y;

		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		switch (m_ProjectionType)
		{
			case ProjectionType::Perspective:
			{
				m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
				break;
			}

			case ProjectionType::Orthographic:
			{
				float OrthoLeft = -m_OrthoSize * m_AspectRatio * 0.5f;
				float OrthoRight = m_OrthoSize * m_AspectRatio * 0.5f;
				float OrthoTop = -m_OrthoSize * 0.5f;
				float OrthoBottom = m_OrthoSize * 0.5f;

				m_Projection = glm::ortho(OrthoLeft, OrthoRight, OrthoTop, OrthoBottom, m_OrthoNear, m_OrthoFar);
				break;
			}
		}

		m_View = glm::translate(glm::mat4(1.0f), m_Position);
		m_View = glm::inverse(m_View);
	}
}
