// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "Origin/Core/MouseEvent.h"
#include "Origin/Core/Event.h"
#include "Origin/Core/Time.h"
#include "Origin/Audio/AudioListener.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

namespace origin
{
    enum CameraStyle
    {
        Pivot = 0,
        FreeMove,
    };

    enum class ProjectionType
    {
        Perspective = 0, Orthographic
    };

    enum class AspectRatioType
    {
        Free = 0,
        SixteenByTen = 1,
        SixteenByNine = 2,
        TwentyOneByNine = 3,
        FourByThree = 4
    };

    class Camera
    {
    public:
        Camera() { }
        virtual ~Camera() { };

        virtual void InitPerspective(float fovy, float aspectRatio, float nearClip, float farClip) { };
        virtual void InitOrthographic(float scale, float nearClip, float farClip) { };
        virtual void OnUpdate(Timestep ts) { };
        virtual void OnEvent(Event &e) { };
        virtual void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateProjection(); UpdateView();}
        virtual void SetProjectionType(ProjectionType type) { m_ProjectionType = type; UpdateProjection(); UpdateView();}
        virtual void SetPosition(const glm::vec3 &position) { m_Position = position; UpdateProjection(); UpdateView();}
        virtual void SetDistance(float distance) { m_Distance = distance; UpdateProjection(); UpdateView();}
        virtual void SetPitch(float pitch) { m_Pitch = pitch; UpdateProjection(); UpdateView();}
        virtual void SetYaw(float yaw) { m_Yaw = yaw; UpdateProjection(); UpdateView();}
        virtual void SetFov(float fov) { m_FOV = fov; UpdateProjection(); UpdateView(); }
        virtual void SetNear(float nearClip) { m_NearClip = nearClip; UpdateProjection(); UpdateView();}
        virtual void SetFar(float farClip) { m_FarClip = farClip; UpdateProjection(); UpdateView();}
        virtual float GetDistance() const { return m_Distance; }
        virtual const glm::vec3 &GetFocalPoint() const { return m_FocalPoint; }
        virtual const glm::vec3 &GetPosition() const { return m_Position; }
        virtual const glm::vec2 GetViewportSize() const { return { m_ViewportWidth, m_ViewportHeight }; }
        virtual glm::vec2 GetOrthoSize() { return m_OrthoSize; }
        virtual const glm::mat4 &GetProjection() const { return m_Projection;  }
        virtual const glm::mat4 &GetViewMatrix() const { return m_View;  }
        virtual const glm::mat4 GetViewProjection() const { return m_Projection * m_View; }
        virtual void SetStyle(CameraStyle style) { m_CameraStyle = style; UpdateProjection(); UpdateView();}
        virtual void SetDraggingActive(bool enable) { m_CanDragging = enable; }
        virtual void SetScrollingActive(bool enable) { m_CanScrolling = enable; }
        virtual void SetMoveActive(bool enable) { m_CanMoving = enable; }
        virtual void SetOrthoScale(float size) { m_OrthoScale = size; UpdateProjection(); UpdateView();}
        virtual void SetOrthoNear(float nearClip) { m_OrthoNearClip = nearClip; UpdateProjection(); UpdateView();}
        virtual void SetOrthoFar(float farClip) { m_OrthoFarClip = farClip; UpdateProjection(); UpdateView();}
        virtual void SetOrthoScaleMax(float size) { m_MaxOrthoScale = size; UpdateProjection(); UpdateView();}
        virtual void SetAspectRatioType(AspectRatioType type) { m_AspecRatioType = type; }
        virtual float GetYaw() const { return m_Yaw; }
        virtual float GetPitch() const { return m_Pitch; }
        virtual float GetFOV() const { return m_FOV; }
        virtual float GetNear() const { return m_NearClip; }
        virtual float GetFar() const { return m_FarClip; }
        virtual float GetAspectRatio() const { return m_AspectRatio; }
        virtual float GetOrthoScale() const { return m_OrthoScale; }
        virtual float GetOrthoNear() const { return m_OrthoNearClip; }
        virtual float GetOrthoFar() const { return m_OrthoFarClip; }
        virtual float GetWidth() const { return m_ViewportWidth; }
        virtual float GetHeight() const { return m_ViewportHeight; }
        virtual bool IsPerspective() const { return m_ProjectionType == ProjectionType::Perspective; }
        virtual void UpdateAudioListener(Timestep ts) { };
        virtual const ProjectionType GetProjectionType() const { return m_ProjectionType; }
        virtual const CameraStyle GetStyle() { return m_CameraStyle; }
        virtual const AspectRatioType GetAspectRatioType() const { return m_AspecRatioType; }
        virtual glm::vec3 GetUpDirection() const { return glm::rotate(glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f)), glm::vec3(0.0f, 1.0f, 0.0f)); }
        virtual glm::vec3 GetRightDirection() const { return glm::rotate(glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f)), glm::vec3(1.0f, 0.0f, 0.0f)); }
        virtual glm::vec3 GetForwardDirection() const { return glm::rotate(glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f)), glm::vec3(0.0f, 0.0f, -1.0f)); }

    protected:
        virtual void UpdateProjection() { };
        virtual void UpdateView() { };

        AudioListener m_AudioListener;
        CameraStyle m_CameraStyle = Pivot;
        ProjectionType m_ProjectionType = ProjectionType::Perspective;
        AspectRatioType m_AspecRatioType = AspectRatioType::Free;
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_FocalPoint = glm::vec3(0.0f);
        glm::vec2 m_InitialMousePosition = glm::vec2(0.0f);
        glm::mat4 m_Projection = glm::mat4(1.0f);
        glm::mat4 m_View = glm::mat4(1.0f);
        glm::vec2 m_OrthoSize = glm::vec2(0.0f);
        bool m_CanDragging = true;
        bool m_CanScrolling = true;
        bool m_CanMoving = true;
        float m_Distance = 8.0f;
        float m_Pitch = 0.0f, m_Yaw = 0.0f;
        float m_ViewportWidth = 1280, m_ViewportHeight = 720;
        float m_MoveSpeed = 2.0f;
        float m_FOV = 45.0f;
        float m_AspectRatio = 1.778f;
        float m_NearClip = 0.1f;
        float m_FarClip = 500.0f;
        float m_OrthoNearClip = -1.0f;
        float m_OrthoFarClip = 100.0f;
        float m_OrthoScale = 10.0f;
        float m_MaxOrthoScale = 100.0f;
    };
}
