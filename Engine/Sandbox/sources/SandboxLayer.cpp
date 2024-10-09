// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Core/KeyCodes.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"

#include "SandboxLayer.h"

#include <iostream>

using namespace origin;

static glm::vec4 clear_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

Ref<SkeletalModel> model;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1500.0f);
    camera.SetAllowedMove(true);

    model = CreateRef<SkeletalModel>("Resources/Models/barbarian.glb");
    for (auto &m : model->GetMeshes())
        OGN_CORE_INFO(m->Name);
}

void SandboxLayer::OnAttach()
{
}

void SandboxLayer::OnUpdate(Timestep ts)
{
    Application& app = Application::Instance();
    camera.OnUpdate(ts, { 0.0f, 0.0f }, { app.GetWindow().GetWidth(), app.GetWindow().GetHeight() });

    RenderCommand::Clear();
    RenderCommand::ClearColor(clear_color);

    Renderer2D::Begin(camera);
    for (i32 i = -10; i < 10; i++)
    {
        Renderer2D::DrawQuad(glm::translate(glm::mat4(1.0f), { i + i * 2.0f, 1.0f, -10.0f }),
            { 1.0f, 0.0f, 0.2f, 1.0f });
    }
    Renderer2D::End();

    MeshRenderer::Begin(camera);
    for (auto& m : model->GetMeshes())
    {
        MeshRenderer::DrawMesh(camera.GetViewProjection(), glm::mat4(1.0f), m->vertexArray);
    }
    MeshRenderer::End();
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

    ImGui::Begin("Test Window");
    ImGui::ColorEdit4("clear color", glm::value_ptr(clear_color));
    ImGui::End();
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