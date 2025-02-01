// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Core/Input.h"
#include "Origin/GUI/UI.h"
#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/FmodSound.h"
#include "Origin/Audio/FmodDsp.h"
#include "SandboxLayer.h"

#include <ImGuizmo.h>
#include <glad/glad.h>

using namespace origin;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
}

void SandboxLayer::OnAttach()
{
    Physics::Init(PhysicsAPI::Jolt);
}

void SandboxLayer::OnUpdate(const Timestep delta_time)
{
}

void SandboxLayer::UpdateCamera(f32 delta_time)
{
    const glm::vec2 &mouse_click_drag_delta = Input::GetMouseClickDragDelta();
}

void SandboxLayer::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<FramebufferResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
    dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseScroll));
}

void SandboxLayer::OnGuiRender()
{
    ImGui::ShowDemoWindow();
}

bool SandboxLayer::OnWindowResize(FramebufferResizeEvent &e)
{
    RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    return false;
}

bool SandboxLayer::OnMouseScroll(MouseScrolledEvent &e)
{
    return false;
}

void SandboxLayer::OnDetach()
{
    Physics::Shutdown();
}