// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef CAMERA_H
#define CAMERA_H

#include "Origin/Core/MouseEvent.h"
#include "Origin/Core/Event.h"
#include "Origin/Core/Time.h"
#include "Origin/Audio/AudioListener.h"
#include "Origin/Renderer/Frustum.h"

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

        virtual void InitPerspective(float fovy, float aspectRatio, float nearClip, float farClip) { }
        virtual void InitOrthographic(float scale, float nearClip, float farClip) { }
        virtual void OnUpdate(Timestep ts, const glm::vec2 &screenMin, const glm::vec2 &screenMax) { }
        virtual void SetProjectionType(ProjectionType type) { }
        virtual void OnEvent(Event &e) { };
        virtual void SetViewportSize(float width, float height) { }
        virtual void SetFov(float fovy) { }
        virtual void SetNear(float nearClip) { }
        virtual void SetFar(float farClip) { }
        virtual void SetOrthoScale(float size) { }
        virtual void SetOrthoNear(float nearClip) { }
        virtual void SetOrthoFar(float farClip) { }
        virtual void SetOrthoScaleMax(float size) { }
        virtual void SetViewMatrix(const glm::mat4 &viewMatrix) { };

        virtual const glm::mat4 GetViewMatrix() const { return glm::mat4(1.0f); }
        virtual const glm::mat4 GetProjectionMatrix() const { return glm::mat4(1.0f); }
        virtual const glm::mat4 GetViewProjection() const { return glm::mat4(1.0f); }
        virtual const glm::vec2 GetViewportSize() const { return glm::vec2(0.0f); }
        virtual const glm::vec3 GetPosition() const { return glm::vec3(0.0f); }
        virtual const glm::vec2 GetOrthoSize() const { return glm::vec2(0.0f); }
        virtual const bool IsPerspective() const { return false; }

        virtual const float GetAspectRatio() const { return 1.0f; }
        virtual const float GetFOV() const { return 0.0f; }
        virtual const float GetNear() const { return 0.0f; }
        virtual const float GetFar() const { return 0.0f; }
        virtual const float GetYaw() const { return 0.0f; }
        virtual const float GetPitch() const { return 0.0f; }
        virtual const float GetOrthoScale() const { return 0.0f; }
        virtual const float GetOrthoNear() const { return 0.0f; }
        virtual const float GetOrthoFar() const { return 0.0f; }
        virtual const ProjectionType GetProjectionType() const { return ProjectionType::Perspective; }

    protected:
        virtual void UpdateProjection() { };
        virtual void UpdateView() { };

        AudioListener m_AudioListener;
        ProjectionType m_ProjectionType = ProjectionType::Perspective;
        AspectRatioType m_AspectRatioType = AspectRatioType::Free;
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_FocalPoint = glm::vec3(0.0f);
        glm::mat4 m_Projection = glm::mat4(1.0f);
        glm::mat4 m_View = glm::mat4(1.0f);
        glm::vec2 m_OrthoSize = glm::vec2(0.0f);
        
        float m_ViewportWidth = 1280, m_ViewportHeight = 720;
        float m_MoveSpeed = 2.0f;
        float m_FOV = 45.0f;
        float m_AspectRatio = 1.778f;
        float m_NearClip = 2.5f;
        float m_FarClip = 50.0f;
        float m_OrthoNearClip = 1.0f;
        float m_OrthoFarClip = 50.0f;
        float m_OrthoScale = 10.0f;
        float m_MaxOrthoScale = 100.0f;
        bool m_ViewMatrixManipulated = false;
    };
}

#endif