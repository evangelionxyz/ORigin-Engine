// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/Asset/AssetImporter.h"
#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/FmodDsp.h"
#include "Origin/Audio/FmodSound.h"
#include "Origin/Core/Input.h"
#include "Origin/EntryPoint.h"
#include "Origin/GUI/UI.h"
#include "SandboxLayer.h"

#include "backends/imgui_impl_vulkan.h"

#include <glad/glad.h>
#include <ImGuizmo.h>

using namespace origin;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    camera.SetPosition({ 0.0f, 0.0f, 0.0f });
}

void SandboxLayer::OnAttach()
{
    Physics::Init(PhysicsAPI::Jolt);
}

void SandboxLayer::OnDetach()
{
    Physics::Shutdown();
}

void SandboxLayer::OnUpdate(const Timestep delta_time)
{
    UpdateCamera(delta_time);
    RenderCommand::Clear();

}

void SandboxLayer::UpdateCamera(f32 delta_time)
{
    const glm::vec2 &mouse_click_drag_delta = Input::GetMouseClickDragDelta();
    camera.OnMouseMove({ mouse_click_drag_delta.x, -mouse_click_drag_delta.y });
    camera.OnUpdate(delta_time);
    camera.UpdateView();
    camera.UpdateProjection();
}

void SandboxLayer::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<FramebufferResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
    dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseScroll));

    camera.OnEvent(e);
}

void SandboxLayer::OnGuiRender()
{
    ImGui::ShowDemoWindow();
}

bool SandboxLayer::OnWindowResize(FramebufferResizeEvent &e)
{
    RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    camera.SetViewportSize(e.GetWidth(), e.GetHeight());
    return false;
}

bool SandboxLayer::OnMouseScroll(MouseScrolledEvent &e)
{
    return false;
}