#include "FPSCamera.h"
#include "Origin\IO\Events\AppEvent.h"
#include "Origin\IO\Input.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\quaternion.hpp>

using namespace Origin;
FPSCamera::FPSCamera(float fov, float aspectRatio)
	: m_Fov(fov), m_AspectRatio(aspectRatio), m_NearClip(0.01f), m_FarClip(100.0f)
{
	m_Position = glm::vec3(0.0f, 0.6f, 5.0f);
	UpdateProjection();
}

void FPSCamera::OnWindowResize(float width, float height)
{
	m_ViewportWidth = width;
	m_ViewportHeight = height;
	m_AspectRatio = m_ViewportWidth / m_ViewportHeight;

	UpdateProjection();
}

void FPSCamera::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	//dispatcher.Dispatch<WindowResizeEvent>(OGN_BIND_EVENT_FN(FPSCamera::OnWindowResize));
	dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(FPSCamera::OnMouseScrolled));
}

bool FPSCamera::OnMouseScrolled(MouseScrolledEvent& e)
{
	float speed = 0.2f;
	m_Position += GetForwardDirection() * e.GetYOffset() * speed;

	UpdateProjection();
	return false;
}

void FPSCamera::OnUpdate(float deltaTime)
{
	// Mouse Input
	const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
	glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
	m_InitialMousePosition = mouse;

	if(Input::IsMouseButtonPressed(Mouse::ButtonRight))
		MouseRotate(delta);

	//if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
		//MousePan(delta);

	if (Input::IsKeyPressed(KeyCode(Key::LeftShift)))
	{
		if (Input::IsKeyPressed(KeyCode(Key::W)))
			m_Position += GetUpDirection() * m_MoveSpeed * deltaTime;
		else if (Input::IsKeyPressed(KeyCode(Key::S)))
			m_Position -= GetUpDirection() * m_MoveSpeed * deltaTime;
	}
	else
	{
		// Key Control
		if (Input::IsKeyPressed(KeyCode(Key::W)))
			m_Position += GetForwardDirection() * m_MoveSpeed * deltaTime;
		else if (Input::IsKeyPressed(KeyCode(Key::S)))
			m_Position -= GetForwardDirection() * m_MoveSpeed * deltaTime;
	}

	if (Input::IsKeyPressed(KeyCode(Key::D)))
		m_Position += GetRightDirection() * m_MoveSpeed * deltaTime;
	else if (Input::IsKeyPressed(KeyCode(Key::A)))
		m_Position -= GetRightDirection() * m_MoveSpeed * deltaTime;

	UpdateProjection();
}

void FPSCamera::MouseRotate(const glm::vec2& delta)
{
	float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
	m_Yaw += yawSign * delta.x * m_RotationSpeed;
	m_Pitch += delta.y * m_RotationSpeed;

	UpdateProjection();
}

glm::vec3 FPSCamera::CalculatePosition() const
{
	return GetForwardDirection();
}

std::pair<float, float> FPSCamera::PanSpeed() const
{
	float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
	float xFactor = 0.0366f * (x * x) - m_AspectRatio * x + 0.5f;

	float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
	float yFactor = 0.0366f * (y * y) - m_AspectRatio * y + 0.5f;

	return { xFactor, yFactor };
}


void FPSCamera::UpdateProjection()
{
	m_Projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);

	glm::quat orientation = GetOrientation();
	m_View = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
	m_View = glm::inverse(m_View);
}

glm::vec3 FPSCamera::GetUpDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 FPSCamera::GetRightDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::vec3 FPSCamera::GetForwardDirection() const
{
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::quat FPSCamera::GetOrientation() const
{
	return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
}

void FPSCamera::MousePan(const glm::vec2& delta)
{
	auto [xSpeed, ySpeed] = PanSpeed();
	m_Position += -GetRightDirection() * delta.x * xSpeed;
	m_Position += GetUpDirection() * delta.y * ySpeed;
}
