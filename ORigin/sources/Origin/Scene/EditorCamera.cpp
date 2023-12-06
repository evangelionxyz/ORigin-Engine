// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "EditorCamera.h"

#include "Origin\Core\Application.h"

#include "Origin/IO/Input.h"
#include "Origin/IO/KeyCodes.h"
#include "Origin/IO/MouseCodes.h"
#include "Origin/Scene/Components.h"
#include "Origin/Scene/Entity.h"

#include <glfw/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

#include <algorithm>

namespace origin {

	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: Camera(m_Projection), m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
	{
		UpdateView();
	}
	
	void EditorCamera::UpdateAudioListener(float deltaTime)
	{
		static glm::vec3 prevPos = GetPosition();
		const glm::vec3& position = GetPosition();
		const glm::vec3 delta = position - prevPos;
		prevPos = position;
		glm::vec3 velocity = (position - prevPos) / glm::vec3(deltaTime);

		m_AudioListener.Set(GetPosition(), velocity, GetForwardDirection(), GetUpDirection());
	}

	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	float EditorCamera::ZoomSpeed() const
	{
		float speed = (m_Distance * m_Distance) * 0.2f;
		speed = std::min(speed, 50.0f);
		speed = std::max(speed, 5.0f);
		
		return speed;
	}

	void EditorCamera::SetViewportSize(float width, float height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		
		UpdateProjection();
	}

	void EditorCamera::UpdateView()
	{
		const glm::quat orientation = GetOrientation();
		m_View = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_View = glm::inverse(m_View);
	}

	std::pair<float, float> EditorCamera::PanSpeed() const
	{
		float xFactor = 0.0f;
		float yFactor = 0.0f;
		
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f);
		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f);
		
		if(m_CameraStyle == Pivot)
		{
			xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.5f * (m_Distance <= 2.0f ? 1.0f : m_Distance);
			yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.5f * (m_Distance <= 2.0f ? 1.0f : m_Distance);
			return { xFactor, yFactor };
		}
		else if(m_CameraStyle == FreeMove)
		{
			xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.5f * 10.0f;
			yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.5f * 10.0f;
			return { xFactor, yFactor };
		}
		
		return { xFactor, yFactor };
	}

	float EditorCamera::RotationSpeed() const
	{
		return 0.8f;
	}

	void EditorCamera::OnUpdate(float deltaTime)
	{
		m_DeltaTime = deltaTime;

		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		const glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
		m_InitialMousePosition = mouse;

		const float wWidth = static_cast<float>(Application::Get().GetWindow().GetWidth());
		const float wHeight = static_cast<float>(Application::Get().GetWindow().GetHeight());

		if (m_EnableMovement)
		{
			glm::vec3 lastPosition = m_LastPosition;
			glm::vec3 velocity(0.0f);

			if (Input::IsMouseButtonPressed(Mouse::ButtonRight) || Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
			{
				if (mouse.x > wWidth - 2.0f)
				{
					m_InitialMousePosition.x = 2.0f;
					Input::SetMousePosition(2.0f, mouse.y);
				}
				else if (mouse.x < 2.0f)
				{
					m_InitialMousePosition.x = wWidth - 2.0f;
					Input::SetMousePosition(wWidth - 2.0f, mouse.y);
				}

				if (mouse.y > wHeight - 2.0f)
				{
					m_InitialMousePosition.y = 2.0f;
					Input::SetMousePosition(mouse.x, 2.0f);
				}
				else if (mouse.y < 2.0f)
				{
					m_InitialMousePosition.y = wHeight - 2.0f;
					Input::SetMousePosition(mouse.x, wHeight - 2.0f);
				}
			}
			switch (m_CameraStyle)
			{
			case origin::Pivot:
				if (Input::IsMouseButtonPressed(Mouse::ButtonRight) && !Input::IsKeyPressed(Key::LeftControl))
					MouseRotate(delta);
				if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle) || (Input::IsMouseButtonPressed(Mouse::ButtonRight) && Input::IsKeyPressed(Key::LeftControl)))
					MousePan(delta);
				m_Position = glm::lerp(m_Position, m_FocalPoint - GetForwardDirection() * m_Distance, deltaTime * 6.0f);
				lastPosition = m_FocalPoint - GetForwardDirection() * m_Distance;
				break;
			case origin::FreeMove:
				if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
					MouseRotate(delta);
				if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
					MousePan(delta);

				if (Input::IsKeyPressed(Key::A))
					velocity -= GetRightDirection();
				else if (Input::IsKeyPressed(Key::D))
					velocity += GetRightDirection();
				if (Input::IsKeyPressed(Key::W))
					velocity += GetForwardDirection();
				else if (Input::IsKeyPressed(Key::S))
					velocity -= GetForwardDirection();

				if (Input::IsKeyPressed(Key::A) || Input::IsKeyPressed(Key::S) || Input::IsKeyPressed(Key::D) || Input::IsKeyPressed(Key::W))
					m_MoveSpeed += deltaTime * 2.0f;
				else
					m_MoveSpeed -= deltaTime * 2.0f;

				if (m_MoveSpeed <= 2.0f)
					m_MoveSpeed = 2.0f;
				else if (m_MoveSpeed >= 20.0f)
					m_MoveSpeed = 20.0f;

				m_Position = glm::lerp(m_Position, m_Position + velocity, deltaTime * m_MoveSpeed);
				lastPosition = m_Position;

				m_Distance = 5.0f;
				m_FocalPoint = lastPosition + GetForwardDirection() * m_Distance;
				break;
			default:
				break;
			}

			m_LastPosition = lastPosition;
		}

		UpdateView();
	}

	void EditorCamera::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(EditorCamera::OnMouseScroll));
	}

	void EditorCamera::SetEntityObject(Entity entity)
	{
		//float currentDistance = m_Distance;
		if(!entity)
		{
			m_TargetPosition = glm::vec3(0.0f);
			//m_Distance = currentDistance;

			return;
		}
		
		const auto& tc = entity.GetComponent<TransformComponent>();
		m_TargetPosition = tc.Translation;

		//m_Distance = glm::lerp(m_Distance, glm::length(m_FocalPoint - m_TargetPosition) / 2.0f, m_DeltaTime * 15.0f);
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset();
		if (m_EnableMovement)
		{
			switch (m_CameraStyle)
			{
			case origin::Pivot:
				MouseZoom(delta * 0.1f);
				UpdateView();
				break;
			case origin::FreeMove:
				m_MoveSpeed += delta;
				break;
			default:
				break;
			}
		}
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		switch (m_CameraStyle)
		{
		case origin::Pivot:
			m_FocalPoint += -GetRightDirection() * delta.x * xSpeed;
			m_FocalPoint += GetUpDirection() * delta.y * ySpeed;
			break;
		case origin::FreeMove:
			m_Position += -GetRightDirection() * delta.x * xSpeed;
			m_Position += GetUpDirection() * delta.y * ySpeed;
			break;
		default:
			break;
		}
		
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(const float delta)
	{
		m_Distance -= delta * ZoomSpeed();
		m_Distance = std::max(m_Distance, 5.0f);
		m_FocalPoint += delta * GetForwardDirection() * ZoomSpeed();
	}

	glm::vec3 EditorCamera::GetUpDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetRightDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::GetForwardDirection() const
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat EditorCamera::GetOrientation() const
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}
}