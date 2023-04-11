#pragma once
#include <glm\glm.hpp>
#include "Origin\IO\Events\Event.h"
#include "Origin\IO\Events\MouseEvent.h"
#include "Origin\IO\Events\KeyEvent.h"

class FPSCamera
{
public:
	FPSCamera() = default;
	FPSCamera(float fov, float aspectRatio);

	glm::mat4& GetViewProjection() { return m_Projection * m_View; }

	void OnUpdate(float deltaTime);
	void OnWindowResize(float width, float height);

	void OnEvent(Origin::Event& event);
	bool OnMouseScrolled(Origin::MouseScrolledEvent& e);

	glm::vec3& GetPosition() { return m_Position; }

private:
	glm::vec3 GetUpDirection() const;
	glm::vec3 GetRightDirection() const;
	glm::vec3 GetForwardDirection() const;
	glm::quat GetOrientation() const;

	void MousePan(const glm::vec2& delta);

	glm::vec3 CalculatePosition() const;
	std::pair<float, float> PanSpeed() const;

private:
	void MouseRotate(const glm::vec2& delta);
	void UpdateProjection();

	float m_AspectRatio;
	float m_ViewportWidth, m_ViewportHeight;
	float m_NearClip, m_FarClip;
	float m_Fov;
	float m_Yaw = 0.0f;
	float m_Pitch = 0.0f;

	float m_MoveSpeed = 2.5f;
	float m_RotationSpeed = 0.5f;

	glm::vec2 m_InitialMousePosition = glm::vec2(0.0f, 0.0f);
	glm::vec3 m_Position;

	glm::mat4 m_View;
	glm::mat4 m_Projection;
};