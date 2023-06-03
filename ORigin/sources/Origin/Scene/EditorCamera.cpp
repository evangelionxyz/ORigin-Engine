// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "EditorCamera.h"

#include "Origin/IO/Input.h"
#include "Origin/IO/KeyCodes.h"
#include "Origin/IO/MouseCodes.h"
#include "Origin/Scene/Component.h"
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
	
	void EditorCamera::UpdateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	float EditorCamera::ZoomSpeed() const
	{
		float speed = (m_Distance * m_Distance) * 0.2f;
		speed = std::min(speed, 50.0f);
		speed = std::max(speed, 1.0f);
		
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

		if (m_EnableMovement)
		{
			// Pivot Camera (rotate based on object)
			if (m_CameraStyle == Pivot)
			{
				if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
					MouseRotate(delta);
				if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
					MousePan(delta);
				
				float moveSpeed = 10.0f;
				m_Position = glm::lerp(m_Position, m_FocalPoint - GetForwardDirection()
					* m_Distance, deltaTime * moveSpeed);
			}
			
			// AWSD Keyboard
			else if (m_CameraStyle == FreeMove)
			{
				auto velocity = glm::vec3(0.0f);
				auto movement = m_Position;
				
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

				movement += velocity;
				m_Position = glm::lerp(glm::vec3(0.0f), movement, deltaTime * m_MoveSpeed);
			}
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
		float currentDistance = m_Distance;
		if(!entity)
		{
			m_TargetPosition = glm::vec3(0.0f);
			m_Distance = currentDistance;

			return;
		}
		
		const auto& tc = entity.GetComponent<TransformComponent>();
		m_TargetPosition = tc.Translation;

		m_Distance = glm::lerp(m_Distance, glm::length(m_FocalPoint - m_TargetPosition) / 2.0f, m_DeltaTime * 15.0f);
	}

	bool EditorCamera::OnMouseScroll(MouseScrolledEvent& e)
	{
		float delta = e.GetYOffset();
		if (m_EnableMovement)
		{
			if (m_CameraStyle == Pivot)
			{
				MouseZoom(delta * 0.1f);
			}
			else if(m_CameraStyle == FreeMove)
			{
				m_MoveSpeed += delta;
			}
			
			UpdateView();
		}
		return false;
	}

	void EditorCamera::MousePan(const glm::vec2& delta)
	{
		auto [xSpeed, ySpeed] = PanSpeed();
		
		m_FocalPoint += -GetRightDirection() * delta.x * xSpeed;
		m_FocalPoint += GetUpDirection() * delta.y * ySpeed;
	}

	void EditorCamera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * RotationSpeed();
		m_Pitch += delta.y * RotationSpeed();
	}

	void EditorCamera::MouseZoom(const float delta)
	{
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