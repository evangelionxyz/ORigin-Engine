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

namespace origin {
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
    virtual ~Camera() { }
    virtual void InitPerspective(f32 fovy, f32 aspectRatio, f32 nearClip, f32 farClip) { }
    virtual void InitOrthographic(f32 scale, f32 nearClip, f32 farClip) { }
    virtual void OnUpdate(Timestep ts) {};
    virtual void SetProjectionType(ProjectionType type) { }
    virtual void OnEvent(Event &e) { };
    virtual void SetViewportSize(u32 width, u32 height) { }
    virtual void SetFov(f32 fovy) { }
    virtual void SetNear(f32 nearClip) { }
    virtual void SetFar(f32 farClip) { }
    virtual void SetOrthoScale(f32 size) { }
    virtual void SetOrthoNear(f32 nearClip) { }
    virtual void SetOrthoFar(f32 farClip) { }
    virtual void SetOrthoScaleMax(f32 size) { }
    virtual void SetViewMatrix(const glm::mat4 &viewMatrix) { };

    virtual glm::mat4 GetViewMatrix() const { return glm::mat4(1.0f); }
    virtual glm::mat4 GetProjectionMatrix() const { return glm::mat4(1.0f); }
    virtual glm::mat4 GetViewProjection() const { return glm::mat4(1.0f); }
    virtual glm::vec2 GetViewportSize() const { return glm::vec2(0.0f); }
    virtual glm::vec3 GetPosition() const { return glm::vec3(0.0f); }
    virtual glm::vec2 GetOrthoSize() const { return glm::vec2(0.0f); }
    virtual bool IsPerspective() const { return false; }

    virtual f32 GetAspectRatio() const { return 1.0f; }
    virtual f32 GetFOV() const { return 0.0f; }
    virtual f32 GetNear() const { return 0.0f; }
    virtual f32 GetFar() const { return 0.0f; }
    virtual f32 GetYaw() const { return 0.0f; }
    virtual f32 GetPitch() const { return 0.0f; }
    virtual f32 GetOrthoScale() const { return 0.0f; }
    virtual f32 GetOrthoNear() const { return 0.0f; }
    virtual f32 GetOrthoFar() const { return 0.0f; }
    virtual ProjectionType GetProjectionType() const { return ProjectionType::Perspective; }

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
        
    u32 m_ViewportWidth = 1280, m_ViewportHeight = 720;
    f32 m_MoveSpeed = 2.0f;
    f32 m_FOV = 45.0f;
    f32 m_AspectRatio = 1.778f;
    f32 m_NearClip = 2.5f;
    f32 m_FarClip = 50.0f;
    f32 m_OrthoNearClip = 1.0f;
    f32 m_OrthoFarClip = 50.0f;
    f32 m_OrthoScale = 10.0f;
    f32 m_MaxOrthoScale = 100.0f;
    bool m_ViewMatrixManipulated = false;
};
}

#endif