// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Core/Input.h"
#include "Origin/GUI/UI.h"

#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/FmodSound.h"
#include "Origin/Audio/FmodDsp.h"

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
f32 total_time_sec = 0.0f;

struct TestModel
{
    Ref<Model> model;
    std::vector<glm::mat4> bone_transforms;

    TestModel() = default;

    TestModel(const std::string &filepath)
    {
        model = Model::Create(filepath);
    }
};

TestModel model_a;
TestModel model_b;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 10000.0f);

    RenderCommand::ClearColor({ 0.125f, 0.125f, 0.125f, 1.0f });
}

void SandboxLayer::OnAttach()
{
    shader = Shader::Create("Resources/Shaders/Skinning.glsl", false, true);
    ubo = UniformBuffer::Create(sizeof(CamData), 0);

    model_a = TestModel("Resources/Models/raptoid.glb");
    model_b = TestModel("Resources/Models/storm_trooper/sss.glb");

    Ref<FmodReverb> reverb = FmodReverb::Create();
    reverb->SetDiffusion(100.0f);
    reverb->SetWetLevel(20.0f);
    reverb->SetDecayTime(5000.0f);

    FMOD::ChannelGroup *reverb_group = FmodAudio::CreateChannelGroup("ReverbGroup");
    reverb_group->setMode(FMOD_CHANNELCONTROL_DSP_TAIL);

    reverb_group->addDSP(0, reverb->GetFmodDsp());

    const Ref<FmodSound> roar_sound = FmodSound::Create("roar", "Resources/Sounds/sound.mp3");
    roar_sound->SetVolume(0.5f);
    roar_sound->AddToChannelGroup(reverb_group);
    roar_sound->Play();
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

    {
        model_a.model->GetBoneTransforms(total_time_sec, model_a.bone_transforms, 0);
        for (auto &mesh : model_a.model->GetMeshes())
        {
            for (const auto &texture : mesh->material.textures)
            {
                if (texture.contains(TextureType::DIFFUSE))
                {
                    texture.at(TextureType::DIFFUSE)->Bind(0);
                    shader->SetInt("udiffuse_texture", 0);
                }
            }
            shader->SetMatrix("umodel_transform", mesh->transform);
            shader->SetMatrix("ubone_transforms", model_a.bone_transforms[0], model_a.bone_transforms.size());
            RenderCommand::DrawIndexed(mesh->vertex_array);
        }
    }

    {
        model_b.model->GetBoneTransforms(total_time_sec, model_b.bone_transforms, 0);
        for (auto &mesh : model_b.model->GetMeshes())
        {
            for (const auto &texture : mesh->material.textures)
            {
                if (texture.contains(TextureType::DIFFUSE))
                {
                    texture.at(TextureType::DIFFUSE)->Bind(0);
                    shader->SetInt("udiffuse_texture", 0);
                }
            }
            shader->SetMatrix("umodel_transform", mesh->transform);
            shader->SetMatrix("ubone_transforms", model_b.bone_transforms[0], model_a.bone_transforms.size());
            RenderCommand::DrawIndexed(mesh->vertex_array);
        }
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
