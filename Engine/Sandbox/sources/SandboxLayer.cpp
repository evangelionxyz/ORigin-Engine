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

struct Data
{
    Ref<Model> model;
    AnimationBlender blender;
    std::vector<SkeletalAnimation> anims;
    glm::vec2 blending_position = { 0.0f, 0.0f };

    Data() = default;
    Data(const std::string &filepath)
    {
        model = Model::Create(filepath);
        anims = model->GetAnimations();
        blender.SetModel(model);
    }
};

Data raptoid;
Data storm_trooper;

i32 model_count = 8;
f32 model_pos_spacing = 2.0f;
Ref<FmodSound> roar_sound;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 10000.0f);
    RenderCommand::ClearColor({ 0.125f, 0.125f, 0.125f, 1.0f });
}

void SandboxLayer::OnAttach()
{
    Physics::Init(PhysicsAPI::Jolt);
    InitSounds();

    shader = Shader::Create("Resources/Shaders/Skinning.glsl", false, true);
    ubo = UniformBuffer::Create(sizeof(CamData), 0);

    storm_trooper = Data("Resources/Models/storm_trooper/storm_trooper.glb");

    raptoid = Data("Resources/Models/raptoid.glb");
    raptoid.blender.AddAnimation(0, { 0.0, 0.0 });
    raptoid.blender.AddAnimation(1, { 2.0, 0.0 });
    raptoid.blender.AddAnimation(2, { 0.0, 2.0 });
    raptoid.blender.AddAnimation(3, { 2.0, 2.0 });
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

    {
        raptoid.blender.UpdateBlending(raptoid.blending_position);
        raptoid.blender.BlendAnimations(ts);
        // 
        //model_a.model->UpdateAnimation(ts, model_a.start_anim_index);
        shader->SetBool("uhas_animation", raptoid.model->HasAnimations());
        shader->SetMatrix("ubone_transforms", raptoid.model->GetBoneTransforms()[0], raptoid.model->GetBoneTransforms().size());
        for (auto &mesh : raptoid.model->GetMeshes())
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

            RenderCommand::DrawIndexed(mesh->vertex_array);
        }
    }

    storm_trooper.model->UpdateAnimation(ts, 0);
    shader->SetBool("uhas_animation", storm_trooper.model->HasAnimations());
    shader->SetMatrix("ubone_transforms", storm_trooper.model->GetBoneTransforms()[0], storm_trooper.model->GetBoneTransforms().size());

    for (i32 x = -model_count; x <= model_count; ++x)
    {
        for (i32 z = -model_count; z <= model_count; ++z)
        {
            for (auto &mesh : storm_trooper.model->GetMeshes())
            {
                for (const auto &texture : mesh->material.textures)
                {
                    if (texture.contains(TextureType::DIFFUSE))
                    {
                        texture.at(TextureType::DIFFUSE)->Bind(0);
                        shader->SetInt("udiffuse_texture", 0);
                    }
                }

                glm::mat4 translation = glm::translate(glm::mat4(1.0f), { x * model_pos_spacing, 0.0f, z * model_pos_spacing });
                shader->SetMatrix("umodel_transform", translation);
                RenderCommand::DrawIndexed(mesh->vertex_array);
            }
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
    ImGui::Begin("Control");

    const f32 &fps = ImGui::GetIO().Framerate;
    ImGui::Text("%.2f", fps);
    ImGui::SliderInt("Model Count x2", &model_count, 0, 100);
    ImGui::SliderFloat("Spacing", &model_pos_spacing, 0.0f, 100.0f);
    ImGui::Separator();

    ImGui::SliderFloat("Blending X", &raptoid.blending_position.x, 0.0f, raptoid.blender.GetMaxSize().x);
    ImGui::SliderFloat("Blending Y", &raptoid.blending_position.y, 0.0f, raptoid.blender.GetMaxSize().y);

    if (ImGui::Button("Play Sound"))
    {
        roar_sound->Play();
    }

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

void SandboxLayer::OnDetach()
{
    Physics::Shutdown();
}

void SandboxLayer::InitSounds()
{
    Ref<FmodReverb> reverb = FmodReverb::Create();
    reverb->SetDiffusion(100.0f);
    reverb->SetWetLevel(20.0f);
    reverb->SetDecayTime(5000.0f);

    FMOD::ChannelGroup *reverb_group = FmodAudio::CreateChannelGroup("ReverbGroup");
    reverb_group->setMode(FMOD_CHANNELCONTROL_DSP_TAIL);

    reverb_group->addDSP(0, reverb->GetFmodDsp());

    roar_sound = FmodSound::Create("roar", "Resources/Sounds/sound.mp3");
    roar_sound->AddToChannelGroup(reverb_group);
    roar_sound->SetVolume(0.5f);
    roar_sound->Play();
}
