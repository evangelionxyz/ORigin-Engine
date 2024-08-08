// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "SceneCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace origin
{
    void SceneCamera::InitPerspective(float fovy, float aspectRatio, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Perspective;
        m_FOV = fovy;
        m_AspectRatio = aspectRatio;
        m_NearClip = nearClip;
        m_FarClip = farClip;

        UpdateView();
        UpdateProjection();
    }

    void SceneCamera::InitOrthographic(float size, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Orthographic;
        m_OrthoScale = size;
        m_OrthoNearClip = nearClip;
        m_OrthoFarClip = farClip;

        UpdateView();
        UpdateProjection();
    }

    void SceneCamera::SetOrthoScale(float scale)
	{
		m_OrthoScale = scale;

		m_OrthoSize.x = m_OrthoScale * m_AspectRatio / 2.0f;
		m_OrthoSize.y = m_OrthoScale / 2.0f;

		UpdateView();
		UpdateProjection();
	}

    void SceneCamera::SetOrthoNear(float nearClip)
    {
        m_OrthoNearClip = nearClip;
    	UpdateView();
    	UpdateProjection();
    }

    void SceneCamera::SetOrthoFar(float farClip)
    {
        m_OrthoFarClip = farClip;
    	UpdateView();
    	UpdateProjection();
    }

    void SceneCamera::SetOrthoScaleMax(float max)
    {
        m_MaxOrthoScale = max;
    	UpdateView();
    	UpdateProjection();
    }

    void SceneCamera::SetAspectRatioType(AspectRatioType type)
    {
		m_AspectRatioType = type;
		SceneCamera::SetViewportSize(m_ViewportWidth, m_ViewportHeight);
    }

    void SceneCamera::SetTransform(const glm::mat4 &transform)
    {
		m_Transform = transform;
    }

    void SceneCamera::SetProjectionType(ProjectionType type)
    {
		m_ProjectionType = type;
        SceneCamera::SetViewportSize(m_ViewportWidth, m_ViewportHeight);
        UpdateView(); 
        UpdateProjection();
    }

    const glm::mat4 SceneCamera::GetViewProjection() const
    {
		return m_Projection * m_View * glm::inverse(m_Transform);
    }

    void SceneCamera::SetViewportSize(float width, float height)
	{
		float desiredRatio = width / height;
		float x = 0, y = 0;

		switch (m_AspectRatioType)
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
		
		if (width / height > desiredRatio)
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

		m_ViewportWidth = x;
		m_ViewportHeight = y;

		m_OrthoSize.x = m_OrthoScale * m_AspectRatio / 2.0f;
		m_OrthoSize.y = m_OrthoScale / 2.0f;

		UpdateView();
		UpdateProjection();
	}

    void SceneCamera::SetFov(float fovy)
    {
        m_FOV = fovy;
    	UpdateView();
    	UpdateProjection();
    }

    void SceneCamera::SetNear(float nearClip)
    {
        m_NearClip = nearClip;
    	UpdateView();
    	UpdateProjection();
    }

    void SceneCamera::SetFar(float farClip)
    {
        m_FarClip = farClip;
    	UpdateView();
    	UpdateProjection();
    }

    void SceneCamera::UpdateProjection()
	{
		switch (m_ProjectionType)
		{
			case ProjectionType::Perspective:
				m_Projection = glm::perspective(m_FOV, m_AspectRatio, m_NearClip, m_FarClip);
				break;
			case ProjectionType::Orthographic:
				m_Projection = glm::ortho(-m_OrthoSize.x, m_OrthoSize.x, -m_OrthoSize.y, m_OrthoSize.y, m_OrthoNearClip, m_OrthoFarClip);
				break;
		}
	}

    void SceneCamera::UpdateView()
    {
        m_View = glm::translate(glm::mat4(1.0f), m_Position);
        m_View = glm::inverse(m_View);
    }

    const glm::mat4 SceneCamera::GetProjectionMatrix() const
    {
        return m_Projection;
    }

    const glm::mat4 SceneCamera::GetViewMatrix() const
    {
        return m_View;
    }

    const glm::vec2 SceneCamera::GetOrthoSize() const
    {
		return m_OrthoSize;
    }

    const glm::vec2 SceneCamera::GetViewportSize() const
    {
        return { m_ViewportWidth, m_ViewportHeight };
    }

    const bool SceneCamera::IsPerspective() const
    {
        return m_ProjectionType == ProjectionType::Perspective;
    }

    const float SceneCamera::GetAspectRatio() const
    {
		return m_AspectRatio;
    }

}
