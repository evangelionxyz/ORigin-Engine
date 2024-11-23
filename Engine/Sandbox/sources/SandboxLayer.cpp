// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"

#include "SandboxLayer.h"

#include <backends/imgui_impl_vulkan.h>

using namespace origin;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
}

void SandboxLayer::OnAttach()
{
}

void SandboxLayer::OnUpdate(Timestep ts)
{
    RenderCommand::Clear();
}

void SandboxLayer::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<FramebufferResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnKeyPressedEvent));

    camera.OnEvent(e);
}

void SandboxLayer::OnGuiRender()
{
    ImGui::ShowDemoWindow(nullptr);
}

bool SandboxLayer::OnWindowResize(FramebufferResizeEvent &e)
{
    RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    camera.SetViewportSize(e.GetWidth(), e.GetHeight());

    return false;
}

bool SandboxLayer::OnKeyPressedEvent(KeyPressedEvent &e)
{
    return false;
}
