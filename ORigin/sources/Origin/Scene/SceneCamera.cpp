// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

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

	void SceneCamera::SetOrthographicScale(float scale)
	{
		m_OrthoScale = scale;

		m_OrthoSize.x = m_OrthoScale * m_AspectRatio / 2.0f;
		m_OrthoSize.y = m_OrthoScale / 2.0f;

		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		float desiredRatio = static_cast<float>(width) / height;
		float x = 0, y = 0;
		switch (m_AspecRatioType)
		{
		case AspectRatioType::SixteenByNine:
			desiredRatio = 16.f / 9.f;
			break;
		case AspectRatioType::SixteenByTen:
			desiredRatio = 16.f / 10.f;
			break;
		case AspectRatioType::FourByThree:
			desiredRatio = 4.f / 3.f;
			break;
		case AspectRatioType::TwentyOneByNine:
			desiredRatio = 21.f / 9.f;
			break;
		}

		m_AspectRatio = desiredRatio;
		
		if (static_cast<float>(width) / height > desiredRatio)
		{
			// set the viewport to max out the height
			y = height;
			x = y * desiredRatio;
		}
		else
		{
			// else, set the viewport to max out the width
			x = width;
			y = x / desiredRatio;
		}

		m_ViewportSize.x = x;
		m_ViewportSize.y = y;

		m_OrthoSize.x = m_OrthoScale * m_AspectRatio / 2.0f;
		m_OrthoSize.y = m_OrthoScale / 2.0f;

		RecalculateProjection();
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
				m_Projection = glm::ortho(-m_OrthoSize.x, m_OrthoSize.x, -m_OrthoSize.y, m_OrthoSize.y, m_OrthoNear, m_OrthoFar);
				break;
		}

		m_View = glm::translate(glm::mat4(1.0f), m_Position);
		m_View = glm::inverse(m_View);
	}
}
