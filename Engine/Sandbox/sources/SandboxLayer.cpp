// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Core/Input.h"
#include "Origin/GUI/UI.h"

#include "SandboxLayer.h"
#include "SkinnedMesh.hpp"

using namespace origin;

struct CamData
{
    glm::mat4 view_projection;
    glm::vec3 positon;
};

CamData cam_data;
Ref<UniformBuffer> ubo;
Ref<Shader> shader;
Ref<Model> model;
Animator animator;
std::vector<ModelAnimation> animations;

f32 total_time_sec = 0.0f;
std::vector<glm::mat4> bone_transforms;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 10000.0f);
    camera.SetPosition({ -198.020416, 144.280899, 81.7220459 });
    camera.SetFocalPoint({ -193.653824, 142.863403, 79.7412262 });
    camera.SetPitch(0.287439466);
    camera.SetYaw(1.14492643);

    RenderCommand::ClearColor({ 0.125f, 0.125f, 0.125f, 1.0f });
}

void SandboxLayer::OnAttach()
{
    shader = Shader::Create("Resources/Shaders/Skinning.glsl", false, true);
    ubo = UniformBuffer::Create(sizeof(CamData), 0);

    std::string filepath = "Resources/Models/raptoid.glb";
    model = Model::Create(filepath);
    //animations = model->GetAnimations();

    //animator.PlayAnimation(&animations[0]);

}

void SandboxLayer::OnUpdate(const Timestep ts)
{
    total_time_sec += ts;

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

    model->GetFinalBoneTransforms(bone_transforms);
    model->UpdateAnimation(ts);

    for (auto &mesh : model->GetMeshes())
    {
        for (const auto &texture : mesh->material.textures)
        {
            if (texture.contains(TextureType::DIFFUSE))
            {
                texture.at(TextureType::DIFFUSE)->Bind(0);
                shader->SetInt("udiffuse_texture", 0);
            }
            /*else if (texture.contains(TextureType::SPECULAR))
            {
                texture.at(TextureType::SPECULAR)->Bind(1);
                shader->SetInt("texture_specular", 1);
            }
            else if (texture.contains(TextureType::NORMALS))
            {
                texture.at(TextureType::NORMALS)->Bind(2);
                shader->SetInt("texture_normals", 2);
            }
            else if (texture.contains(TextureType::BASE_COLOR))
            {
                texture.at(TextureType::BASE_COLOR)->Bind(3);
                shader->SetInt("texture_base_color", 3);
            }*/
        }


        shader->SetMatrix("umodel_transform", mesh->transform);
        shader->SetMatrix("ubone_transforms", bone_transforms[0], bone_transforms.size());
        RenderCommand::DrawIndexed(mesh->vertex_array);
    }
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
