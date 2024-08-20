// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "EditorCamera.h"
#include "Origin/Core/Application.h"
#include "Origin/Core/Input.h"
#include "Origin/Core/KeyCodes.h"
#include "Origin/Core/MouseCodes.h"
#include "Origin/Scene/Components/Components.h"
#include "Origin/Scene/Entity.h"
#include <GLFW/glfw3.h>
#include <algorithm>

namespace origin {

	void EditorCamera::InitPerspective(float fovy, float aspectRatio, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_FOV = fovy;
		m_AspectRatio = aspectRatio;
		m_NearClip = nearClip;
		m_FarClip = farClip;

		UpdateView();
		UpdateProjection();
	}

	void EditorCamera::InitOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthoScale = size;
		m_OrthoNearClip = nearClip;
		m_OrthoFarClip = farClip;

		UpdateView();
		UpdateProjection();
	}

    void EditorCamera::SetProjectionType(ProjectionType type)
    {
		m_ProjectionType = type;

		UpdateView();
		UpdateProjection();
    }

    void EditorCamera::UpdateAudioListener(Timestep ts)
	{
		m_AudioListener.Set(m_Position, glm::vec3(0.0f), GetForwardDirection(), GetUpDirection());
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;

		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:	
			m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
			break;

		case ProjectionType::Orthographic:
			m_OrthoSize.x = m_OrthoScale * m_AspectRatio / 2.0f;
			m_OrthoSize.y = m_OrthoScale / 2.0f;
			m_Projection = glm::ortho(-m_OrthoSize.x, m_OrthoSize.x, -m_OrthoSize.y, m_OrthoSize.y, m_OrthoNearClip, m_OrthoFarClip);
			break;
		}
	}

	float EditorCamera::GetZoomSpeed() const
	{
		float speed = 0.0f;
		float distance = 0.0f;
		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
			distance = m_Distance * 0.2f;
			distance = std::max(distance, 0.0f);
			speed = distance * distance;
			speed = std::min(speed, 100.0f);
			break;
		case ProjectionType::Orthographic:
			speed = m_OrthoScale * 0.5f;
			break;
		}
		
		return speed;
	}

	void EditorCamera::SetViewportSize(float width, float height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		UpdateProjection();
	}

    void EditorCamera::SetFov(float fovy)
    {
		m_FOV = fovy;
    }

    void EditorCamera::SetNear(float nearClip)
    {
		m_NearClip = nearClip;
    }

    void EditorCamera::SetFar(float farClip)
    {
		m_FarClip = farClip;
    }

    void EditorCamera::SetPosition(const glm::vec3 &position)
    {
		m_Position = position;
		UpdateProjection();
		UpdateView();
    }

    void EditorCamera::SetFocalPoint(const glm::vec3 &position)
    {
		m_FocalPoint = position;
    }

    void EditorCamera::SetStyle(CameraStyle style)
    {
		m_CameraStyle = style;
    }

    void EditorCamera::SetPitch(float pitch)
    {
		m_Pitch = pitch;
    }

    void EditorCamera::SetYaw(float yaw)
    {
		m_Yaw = yaw;
    }

    void EditorCamera::UpdateView()
	{
		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
			m_View = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(glm::quat({ -m_Pitch, -m_Yaw, 0.0f }));
			m_View = glm::inverse(m_View);
			break;
		case ProjectionType::Orthographic:
			m_View = glm::translate(glm::mat4(1.0f), m_Position);
			m_View = glm::inverse(m_View);
			break;
		}
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float xFactor = 0.0f;
		float yFactor = 0.0f;

		float x = std::min(m_ViewportWidth * 0.01f, 1.8f);
		float y = std::min(m_ViewportHeight * 0.01f, 1.8f);

        xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;
        yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	void EditorCamera::OnUpdate(Timestep ts, const glm::vec2 &screenMin, const glm::vec2 &screenMax)
	{
        const glm::vec2 mouse { Input::Get().GetMouseX(), Input::Get().GetMouseY() };
		const glm::vec2 delta = (mouse - m_LastMousePos) * 0.003f;
        m_LastMousePos = mouse;

		glm::vec3 input(0.0f);
        static glm::vec3 lastPosition = glm::vec3(0.0f);

		if (m_AllowedMove)
		{
            bool panOrRotate = Input::Get().IsMouseButtonPressed(Mouse::ButtonRight) || Input::Get().IsMouseButtonPressed(Mouse::ButtonMiddle);

            float offset = 1.0f;
            int windowPosX, windowPosY;
            glfwGetWindowPos((GLFWwindow *)Input::Get().GetWindow(), &windowPosX, &windowPosY);
            glm::vec2 absMousePos = mouse + glm::vec2(windowPosX, windowPosY);

            m_IsInViewport = absMousePos.x < screenMax.x && absMousePos.x > screenMin.x
                && absMousePos.y < screenMax.y && absMousePos.y > screenMin.y;

            if (panOrRotate && m_IsInViewport)
            {
                glm::vec2 viewportCenter = (screenMin + screenMax) / 2.0f;
                Input::Get().SetMousePosition(viewportCenter.x - windowPosX, viewportCenter.y - windowPosY);
				m_LastMousePos = viewportCenter - glm::vec2(windowPosX, windowPosY);
            }

            Input::Get().SetMouseHide(panOrRotate && m_IsInViewport);
            if (m_IsInViewport)
            {
                if (m_ProjectionType == ProjectionType::Perspective)
                {
                    switch (m_CameraStyle)
                    {
                    case CameraStyle::Pivot:
						if (Input::Get().IsMouseButtonPressed(Mouse::ButtonRight) && !Input::Get().IsKeyPressed(Key::LeftControl))
						{
                            MouseRotate(delta, ts);
						}

						if (Input::Get().IsMouseButtonPressed(Mouse::ButtonMiddle) || (Input::Get().IsMouseButtonPressed(Mouse::ButtonRight)
							&& Input::Get().IsKeyPressed(Key::LeftControl)))
						{
                            MousePan(delta);
						}

                        m_Position = glm::lerp(m_Position, m_FocalPoint - GetForwardDirection() * m_Distance, 0.8f);
                        lastPosition = m_FocalPoint - GetForwardDirection() * m_Distance;
                        break;

                    case CameraStyle::FreeMove:
						if (Input::Get().IsMouseButtonPressed(Mouse::ButtonRight))
						{
                            MouseRotate(delta, ts);
						}

						if (Input::Get().IsMouseButtonPressed(Mouse::ButtonMiddle))
						{
                            MousePan(delta);
						}

                        if (!Input::Get().IsKeyPressed(Key::LeftControl))
                        {
							if (Input::Get().IsKeyPressed(Key::A)) input -= GetRightDirection();
                            if (Input::Get().IsKeyPressed(Key::D)) input += GetRightDirection();
                            if (Input::Get().IsKeyPressed(Key::W)) input += GetForwardDirection();
                            if (Input::Get().IsKeyPressed(Key::S)) input -= GetForwardDirection();
                        }
                        break;
                    }
                }
                else if (m_ProjectionType == ProjectionType::Orthographic)
                {
					if (Input::Get().IsMouseButtonPressed(Mouse::ButtonMiddle) || (Input::Get().IsMouseButtonPressed(Mouse::ButtonRight)
						&& Input::Get().IsKeyPressed(Key::LeftControl)))
					{
                        MousePan(delta);
					}

                    lastPosition = m_Position;
                }
            }
		}

		if (m_ProjectionType == ProjectionType::Perspective)
		{
			switch (m_CameraStyle)
			{
			case CameraStyle::Pivot:
				m_Position = glm::lerp(m_Position, m_FocalPoint - GetForwardDirection() * m_Distance, 0.8f);
				lastPosition = m_FocalPoint - GetForwardDirection() * m_Distance;
				break;
			case CameraStyle::FreeMove:
			{
                if (glm::length(input) > 0.0f)
                {
                    input = glm::normalize(input);
                    m_Velocity += input * ACCELERATION * (float)ts;
                }
                else
                {
                    // Decelerate when no input
                    float speed = glm::length(m_Velocity);
                    if (speed > 0.0f)
                    {
                        glm::vec3 deceleration = -glm::normalize(m_Velocity) * DECELERATION * (float)ts;
                        if (glm::length(deceleration) > speed)
                        {
                            m_Velocity = glm::vec3(0.0f);
                        }
                        else
                        {
                            m_Velocity += deceleration;
                        }
                    }
                }

                // Clamp velocity to maximum speed
                float speed = glm::length(m_Velocity);
                if (speed > MAX_SPEED)
                {
                    m_Velocity = glm::normalize(m_Velocity) * MAX_SPEED;
                }
				m_Position += m_Velocity * (float)ts;
                lastPosition = m_Position;
                m_Distance = 5.0f;
                m_FocalPoint = lastPosition + GetForwardDirection() * m_Distance;
				break;
			}
			}
		}
        else if (m_ProjectionType == ProjectionType::Orthographic)
        {
            lastPosition = m_Position;
        }

		UpdateView();
		UpdateProjection();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	void EditorCamera::SetOrthoScale(float value)
	{
		m_OrthoScale = value;
	}

	void EditorCamera::SetOrthoScaleMax(float max)
	{
		m_MaxOrthoScale = max;
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		if (!m_IsInViewport)
		{
			return false;
		}

		float delta = e.GetYOffset();
		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
			if (m_CameraStyle == Pivot)
			{
				MouseZoom(delta * 0.1f);
				UpdateView();
				UpdateProjection();
			}
			else if (m_CameraStyle == FreeMove)
			{
				if (Input::Get().IsMouseButtonPressed(Mouse::ButtonRight))
				{
					m_MoveSpeed += delta * 5.0f;
					m_MoveSpeed = glm::clamp(m_MoveSpeed, 0.01f, MAX_SPEED);
				}
				else
				{
					m_Position = (m_Position + GetForwardDirection() * delta * m_MoveSpeed);
				}
			}
			break;
		case ProjectionType::Orthographic:
			MouseZoom(delta * 0.1f);
			UpdateView();
			UpdateProjection();
			break;
		}

		return true;
    }

    void EditorCamera::MousePan(const glm::vec2 &delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();

		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
			if (m_CameraStyle == Pivot)
			{
				m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_Distance;
				m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_Distance;
			}
			else
			{
				m_Position -= GetRightDirection() * delta.x * m_MoveSpeed;
				m_Position += GetUpDirection() * delta.y * m_MoveSpeed;
			}
			break;
		case ProjectionType::Orthographic:
			m_Position += glm::vec3(-delta.x * (m_OrthoScale / m_ViewportHeight) * 300.0f, delta.y * (m_OrthoScale / m_ViewportHeight) * 300.0f, 0.0f);
			break;
		}
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta, float dt)
    {
        float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
        glm::vec2 acceleration = delta * SENSITIVITY;
        m_AngularVelocity += acceleration;
        m_AngularVelocity *= std::pow(DAMPING, dt);
        m_Yaw += yawSign * m_AngularVelocity.x * RotationSpeed() * 2.0f;
        m_Pitch += m_AngularVelocity.y * RotationSpeed() * 2.0f;
        m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
	}

	void EditorCamera::MouseZoom(const float delta)
	{
		switch (m_ProjectionType)
		{
		case ProjectionType::Perspective:
			m_Distance -= delta * GetZoomSpeed();
			if (m_Distance <= 1.0f)
			{
				m_FocalPoint += GetForwardDirection();
				m_Distance = 1.0f;
			}
			break;
		case ProjectionType::Orthographic:
			m_OrthoScale -= delta * GetZoomSpeed();
			m_OrthoScale = glm::clamp(m_OrthoScale, 1.0f, m_MaxOrthoScale);
			break;
		}
	}

	const glm::vec3 EditorCamera::GetPosition() const
	{
		return m_Position;
	}

	const glm::vec2 EditorCamera::GetViewportSize() const
	{
		return { m_ViewportWidth, m_ViewportHeight };
	}

    void EditorCamera::SetAllowedMove(bool active)
    {
		m_AllowedMove = active;
    }

    void EditorCamera::SetDistance(float distance)
    {
		m_Distance = distance;
    }

    void EditorCamera::SetViewMatrix(const glm::mat4 &viewMatrix)
    {
		m_View = viewMatrix;

        glm::mat4 inverseView = glm::inverse(m_View);

        glm::vec3 skew;
        glm::vec4 perspective;
        glm::quat orientation;
        glm::vec3 scale;

        glm::decompose(inverseView, scale, orientation, m_Position, skew, perspective);

        // Extract yaw and pitch from the orientation quaternion
        glm::vec3 euler = glm::eulerAngles(orientation);
        m_Yaw = -euler.y;
        m_Pitch = -euler.x;
    }

    const glm::mat4 EditorCamera::GetProjectionMatrix() const
	{
		return m_Projection;
	}

	const glm::mat4 EditorCamera::GetViewMatrix() const
	{
		return m_View;
	}

    const glm::mat4 EditorCamera::GetViewProjection() const
    {
		return m_Projection * m_View;
    }

}