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
void EditorCamera::InitPerspective(const f32 fovy, const f32 aspect_ratio, const f32 near_clip, const f32 far_clip)
{
    m_ProjectionType = ProjectionType::Perspective;
    m_FOV = fovy;
    m_AspectRatio = aspect_ratio;
    m_NearClip = near_clip;
    m_FarClip = far_clip;

    UpdateView();
    UpdateProjection();
}

void EditorCamera::InitOrthographic(f32 size, f32 nearClip, f32 farClip)
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

void EditorCamera::OnUpdate(Timestep ts)
{
    glm::vec3 movement_direction(0.0f);

    if (m_ProjectionType == ProjectionType::Perspective)
    {
        switch (m_camera_style)
        {
        case CameraStyle::FreeMove:
        {
            if (!Input::IsKeyPressed(Key::LeftControl))
            {
                if (Input::IsKeyPressed(Key::A))
                    movement_direction -= GetRightDirection();
                if (Input::IsKeyPressed(Key::D))
                    movement_direction += GetRightDirection();
                if (Input::IsKeyPressed(Key::W))
                    movement_direction += GetForwardDirection();
                if (Input::IsKeyPressed(Key::S))
                    movement_direction -= GetForwardDirection();
            }

            if (length(movement_direction) > 0.0f)
            {
                movement_direction = normalize(movement_direction);
                m_velocity += movement_direction * ACCELERATION * (f32)ts;
            }
            else
            {
                // Decelerate when no input
                const f32 speed = length(m_velocity);
                if (speed > 0.0f)
                {
                    glm::vec3 deceleration = -normalize(m_velocity) * DECELERATION * (f32)ts;
                    if (length(deceleration) > speed)
                    {
                        m_velocity = glm::vec3(0.0f);
                    }
                    else
                    {
                        m_velocity += deceleration;
                    }
                }
            }

            // Clamp velocity to maximum speed
            f32 speed = glm::length(m_velocity);
            if (speed > MAX_SPEED)
            {
                m_velocity = glm::normalize(m_velocity) * MAX_SPEED;
            }
            m_Position += m_velocity * static_cast<f32>(ts);
            m_last_switch_position = m_Position;
            m_distance = 5.0f;
            m_FocalPoint = m_last_switch_position + GetForwardDirection() * m_distance;
            break;
        }
        }
    }
}

void EditorCamera::UpdateAudioListener(Timestep ts)
{
    m_AudioListener.Set(m_Position, glm::vec3(0.0f), GetForwardDirection(), GetUpDirection());
}

void EditorCamera::UpdateProjection()
{
    m_AspectRatio = static_cast<f32>(m_ViewportWidth) / static_cast<f32>(m_ViewportHeight);

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

f32 EditorCamera::GetZoomSpeed() const
{
    f32 speed = 0.0f;
    switch (m_ProjectionType)
    {
    case ProjectionType::Perspective:
    {
        f32 distance = m_distance * 0.2f;
        distance = std::max(distance, 0.0f);
        speed = distance * distance;
        speed = std::min(speed, 100.0f);
    } break;
    case ProjectionType::Orthographic:
        speed = m_OrthoScale * 0.5f;
        break;
    }
        
    return speed;
}

void EditorCamera::SetViewportSize(u32 width, u32 height)
{
    m_ViewportWidth = width;
    m_ViewportHeight = height;

    UpdateProjection();
}

void EditorCamera::SetFov(f32 fovy)
{
    m_FOV = fovy;
}

void EditorCamera::SetNear(f32 nearClip)
{
    m_NearClip = nearClip;
}

void EditorCamera::SetFar(f32 farClip)
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
    m_camera_style = style;
}

void EditorCamera::SetPitch(f32 pitch)
{
    m_pitch = pitch;
}

void EditorCamera::SetYaw(f32 yaw)
{
    m_yaw = yaw;
}

void EditorCamera::UpdateView()
{
    switch (m_ProjectionType)
    {
    case ProjectionType::Perspective:
        m_View = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(glm::quat({ -m_pitch, -m_yaw, 0.0f }));
        m_View = glm::inverse(m_View);
        break;
    case ProjectionType::Orthographic:
        m_View = glm::translate(glm::mat4(1.0f), m_Position);
        m_View = glm::inverse(m_View);
        break;
    }
}

std::pair<f32, f32> EditorCamera::PanSpeed() const
{
    const f32 x = std::min(m_ViewportWidth * 0.01f, 1.8f);
    const f32 y = std::min(m_ViewportHeight * 0.01f, 1.8f);
    const f32 x_factor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;
    const f32 y_factor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;
    return { x_factor, y_factor };
}

f32 EditorCamera::RotationSpeed()
{
    return 0.8f;
}

void EditorCamera::SetOrthoScale(f32 value)
{
    m_OrthoScale = value;
}

void EditorCamera::SetOrthoScaleMax(f32 max)
{
    m_MaxOrthoScale = max;
}

void EditorCamera::OnMouseScroll(f32 delta)
{
    switch (m_ProjectionType)
    {
    case ProjectionType::Perspective:
        if (m_camera_style == Pivot)
        {
            MouseZoom(delta * 0.1f);
        }
        else if (m_camera_style == FreeMove)
        {
            if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
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
        break;
    }
}

void EditorCamera::OnMouseMove(const glm::vec2 &delta)
{
    if (m_ProjectionType == ProjectionType::Perspective)
    {
        switch (m_camera_style)
        {
        case CameraStyle::Pivot:
        {
            if (Input::IsMouseButtonPressed(Mouse::ButtonRight) && !Input::IsKeyPressed(Key::LeftControl))
                MouseRotate(delta);
            if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle) || (Input::IsMouseButtonPressed(Mouse::ButtonRight) && Input::IsKeyPressed(Key::LeftControl)))
                MousePan(delta);
            break;
        }
        case CameraStyle::FreeMove:
        {
            if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
                MouseRotate(delta);
            if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
                MousePan(delta);
            break;
        }
        }
    }
    else if (m_ProjectionType == ProjectionType::Orthographic)
    {
        if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle) || (Input::IsMouseButtonPressed(Mouse::ButtonRight) && Input::IsKeyPressed(Key::LeftControl)))
            MousePan(delta);
    }
}

void EditorCamera::MousePan(const glm::vec2 &delta)
{
    const auto &[xSpeed, ySpeed] = PanSpeed();

    switch (m_ProjectionType)
    {
    case ProjectionType::Perspective:
        if (m_camera_style == Pivot)
        {
            m_FocalPoint += -GetRightDirection() * delta.x * xSpeed * m_distance;
            m_FocalPoint += GetUpDirection() * delta.y * ySpeed * m_distance;
        }
        else
        {
            m_Position += GetRightDirection() * delta.x * xSpeed * 0.1f;
            m_Position -= GetUpDirection() * delta.y * ySpeed * 0.1f;
        }
        break;
    case ProjectionType::Orthographic:
        m_Position += glm::vec3(delta.x * (m_OrthoScale / m_ViewportHeight), -delta.y * (m_OrthoScale / m_ViewportHeight), 0.0f);
        break;
    }
}

void EditorCamera::MouseRotate(const glm::vec2& delta)
{
    const auto &[xSpeed, ySpeed] = PanSpeed();

    const f32 yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
    m_yaw += yawSign * RotationSpeed() * -delta.x * xSpeed * 0.03f;
    m_pitch += RotationSpeed() * -delta.y * ySpeed * 0.03f;
    m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
}

void EditorCamera::MouseZoom(const f32 delta)
{
    switch (m_ProjectionType)
    {
    case ProjectionType::Perspective:
        m_distance -= delta * GetZoomSpeed();
        if (m_distance <= 1.0f)
        {
            m_FocalPoint += GetForwardDirection();
            m_distance = 1.0f;
        }
        break;
    case ProjectionType::Orthographic:
        m_OrthoScale -= delta * GetZoomSpeed();
        m_OrthoScale = glm::clamp(m_OrthoScale, 1.0f, m_MaxOrthoScale);
        break;
    }

    UpdateView();
    UpdateProjection();
}

glm::vec3 EditorCamera::GetPosition() const
{
    return m_Position;
}

glm::vec2 EditorCamera::GetViewportSize() const
{
    return { m_ViewportWidth, m_ViewportHeight };
}

void EditorCamera::SetDistance(f32 distance)
{
    m_distance = distance;
}

void EditorCamera::SetViewMatrix(const glm::mat4 &viewMatrix)
{
    m_View = viewMatrix;
    glm::mat4 inverse_view = inverse(m_View);
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::quat orientation;
    glm::vec3 scale;
    decompose(inverse_view, scale, orientation, m_Position, skew, perspective);
    const glm::vec3 euler = eulerAngles(orientation);
    m_yaw = -euler.y;
    m_pitch = -euler.x;
}

glm::mat4 EditorCamera::GetProjectionMatrix() const
{
    return m_Projection;
}

glm::mat4 EditorCamera::GetViewMatrix() const
{
    return m_View;
}

glm::mat4 EditorCamera::GetViewProjection() const
{
    return m_Projection * m_View;
}
}