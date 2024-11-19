// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"

#include "SandboxLayer.h"

using namespace origin;

static glm::vec4 clear_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

u32 SandboxLayer::LoadTexture(const unsigned char *data, int width, int height)
{
    u32 texture_id;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
    glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    constexpr u32 level = 0, x_offset = 0, y_offset = 0;
    glTextureSubImage2D(texture_id, level, x_offset, y_offset, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    return texture_id;
}

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    m_Textures.push_back(LoadTexture(logo_black_data, logo_black_width, logo_black_height));
}

void SandboxLayer::OnAttach()
{
}

void SandboxLayer::OnUpdate(Timestep ts)
{
    Application& app = Application::GetInstance();
    camera.OnUpdate(ts);

    RenderCommand::Clear();
    RenderCommand::ClearColor(clear_color);
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

    for (const u32 tex : m_Textures)
    {
        ImGui::Image(reinterpret_cast<ImTextureID>(tex), { 50.0f, 50.0f });
    }
    
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