// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Core/Input.h"
#include "Origin/GUI/UI.h"

#include "SandboxLayer.h"
#include "SM.hpp"

using namespace origin;

struct CamData
{
    glm::mat4 view_projection;
    glm::vec3 positon;
};

Ref<Shader> shader;
Ref<SkinnedMesh> skinned_mesh;
i32 display_bone = 0;
CamData cam_data;
Ref<UniformBuffer> ubo;


SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 10000.0f);
    camera.SetPosition({ 0.0f, 2.0f, 8.0f });

    std::string filepath = "Resources/Models/raptoid.glb";
    skinned_mesh = CreateRef<SkinnedMesh>();
    if (!skinned_mesh->LoadMesh(filepath))
    {
        OGN_CORE_ASSERT(false, "Failed to load mesh {}", filepath);
        return;
    }

    shader = Shader::Create("Resources/Shaders/Skinning.glsl", false, true);
    ubo = UniformBuffer::Create(sizeof(CamData), 0);

    // s_data.model = Model("Resources/Models/kay_kit/Characters/gltf/Knight.glb");
    // s_data.model = Model("Resources/Models/Test/Test.glb");
    // s_data.model = Model("Resources/Models/storm_trooper/sss.glb");
    // s_data.model = Model("Resources/Models/cube_plane.glb");
    // s_data.model = Model("Resources/Models/survival_guitar_backpack.glb");
    // s_data.model = Model("Resources/Models/raptoid.glb");
    RenderCommand::ClearColor({ 0.3f,0.3f,0.3f, 1.0f });
}

void SandboxLayer::OnAttach()
{
}

void SandboxLayer::OnUpdate(const Timestep ts)
{
    RenderCommand::Clear();
    const glm::vec2 &delta = Input::GetMouseClickDragDelta();
    camera.OnMouseMove(delta);
    camera.OnUpdate(ts);

    camera.UpdateView();
    camera.UpdateProjection();

    cam_data.view_projection = camera.GetViewProjection();
    cam_data.positon = camera.GetPosition();

    ubo->Bind();
    ubo->SetData(&cam_data, sizeof(CamData));

    shader->Enable();
    shader->SetInt("udisplay_bone_index", display_bone);
    shader->SetMatrix("umodel", glm::mat4(1.0f));

    skinned_mesh->Render();

}

void SandboxLayer::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<FramebufferResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnKeyPressed));
    dispatcher.Dispatch<MouseMovedEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseMove));
    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseButtonPressed));
    dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseScroll));

    camera.OnEvent(e);
}

void SandboxLayer::OnGuiRender()
{
    ImGui::Begin("Control");
    ImGui::SliderInt("Select Bone", &display_bone, 0, skinned_mesh->NumBones());
    const f32 &fps = ImGui::GetIO().Framerate;
    ImGui::Text("FPS %.2f", fps);
    ImGui::End();
}

bool SandboxLayer::OnWindowResize(FramebufferResizeEvent &e)
{
    RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    camera.SetViewportSize(e.GetWidth(), e.GetHeight());

    return false;
}

bool SandboxLayer::OnKeyPressed(KeyPressedEvent &e)
{
    return false;
}

bool SandboxLayer::OnMouseButtonPressed(MouseButtonPressedEvent &e)
{
    return false;
}

bool SandboxLayer::OnMouseMove(MouseMovedEvent &e)
{
   
    return false;
}

bool SandboxLayer::OnMouseScroll(MouseScrolledEvent &e)
{
    camera.OnMouseScroll(e.GetYOffset());

    return false;
}
