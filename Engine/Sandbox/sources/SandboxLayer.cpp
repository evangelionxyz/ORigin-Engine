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

#include <glad/glad.h>

using namespace origin;

struct CamData
{
    glm::mat4 view_projection;
    glm::vec3 positon;
};

CamData cam_data;
Ref<UniformBuffer> ubo;
Ref<Shader> shader;
Ref<Shader> skybox_shader;

struct Data
{
    Ref<Model> model;
    AnimationBlender blender;
    std::vector<SkeletalAnimation> anims;
    glm::vec2 blending_position = { 180.0f, 0.0f };

    f32 idling = 2.0f;
    f32 threatening = 0.0f;
    f32 retreating = 0.0f;
    f32 running = 0.0f;

    Data() = default;
    Data(const std::string &filepath)
    {
        model = Model::Create(filepath);
        anims = model->GetAnimations();
        blender.SetModel(model);
    }
};

Data raptoid;
f32 animation_speed_playback = 1.0f;
f32 increment_speed = 300.0f;
f32 decrement_speed = 800.0f;
f32 target_direction = 180.0f;
f32 target_speed = 0.0f;
Ref<Skybox> skybox;
f32 blur_factor = 0.005f;

i32 model_count = 8;
f32 model_pos_spacing = 2.0f;
Ref<FmodSound> roar_sound;


SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 10000.0f);
    camera.SetPosition({ -1.62620163f, 1.37793064f,2.60992050f });
    camera.SetPitch(0.161835521f);
    camera.SetYaw(0.640095532f);
    RenderCommand::ClearColor({ 0.125f, 0.125f, 0.125f, 1.0f });
}

void SandboxLayer::OnAttach()
{
    Physics::Init(PhysicsAPI::Jolt);
    InitSounds();

    shader = Shader::Create("Resources/Shaders/Skinning.glsl", false, true);
    skybox_shader = Shader::Create("Resources/Shaders/Skybox.glsl", false, true);

    ubo = UniformBuffer::Create(sizeof(CamData), 0);

    raptoid = Data("Resources/Models/base_character.glb");
    raptoid.blender.SetRange({ 0.0f, 0.0f }, { 360.0f, 400.0f });

    raptoid.blender.AddAnimation("Idling", { 0.0f, 0.0f }, { 72.0f, 25.0f }); // idle
    raptoid.blender.AddAnimation("Idling", { 72.0f, 0.0f }, { 144.0f, 25.0f }); // idle
    raptoid.blender.AddAnimation("Idling", { 144.0f, 0.0f }, { 216.0f, 25.0f }); // idle
    raptoid.blender.AddAnimation("Idling", { 216.0f, 0.0f }, { 288.0f, 25.0f }); // idle
    raptoid.blender.AddAnimation("Idling", { 288.0f, 0.0f }, { 360.0f, 25.0f }); // idle

    raptoid.blender.AddAnimation("Walking_Forward", { 72.0f, 50.0f }, { 180.0f, 150.0f }); // walking
    raptoid.blender.AddAnimation("Walking_Forward", { 180.0f, 50.0f }, { 288.0f, 150.0f }); // walking

    raptoid.blender.AddAnimation("Walking_Backward", { 0.0f, 50.0f }, { 72.0f, 150.0f }); // walking backward
    raptoid.blender.AddAnimation("Running_Backward", { 0.0f, 200.0f }, { 120.0f, 400.0f }); // backward
    raptoid.blender.AddAnimation("Running_Forward", { 144.0f, 180.0f }, { 216.0f, 400.0f }); // forward
    raptoid.blender.AddAnimation("Running_Backward", { 230.0f, 200.0f }, { 360.0f, 400.0f }); // backward
    raptoid.blender.AddAnimation("Walking_Backward", { 288.0f, 50.0f }, { 360.0f, 150.0f }); // walking backward

    skybox = Skybox::Create("Resources/Skybox", ".jpg");
}

void SandboxLayer::OnUpdate(const Timestep delta_time)
{
    constexpr f32 WALK_FORWARD_SPEED = 72.0f;
    constexpr f32 WALK_BACKWARD_SPEED = 50.0f;
    constexpr f32 RUN_SPEED = 400.0f;
    constexpr f32 IDLE_SPEED = 0.0f;

    constexpr f32 WALK_BACKWARD_DIRECTION = 0.0f; // or 360.0f
    constexpr f32 RUN_BACKWARD_DIRECTION = 72.0f; // or 216.0f
    constexpr f32 FORWARD_DIRECTION = 180.0f;

    constexpr f32 MOVE_INCREMENT_SPEED = 500.0f;
    constexpr f32 DIRECTION_INCREMENT_SPEED = 900.0f;
    constexpr f32 RUN_INCREMENT_SPEED = 800.0f;
    constexpr f32 DECREMENT_SPEED = 800.0f;

    // Define target values for speed and direction
    f32 target_speed = IDLE_SPEED;
    f32 target_direction = raptoid.blending_position.x;
    f32 move_increment_speed = MOVE_INCREMENT_SPEED;
    f32 direction_increment_speed = DIRECTION_INCREMENT_SPEED;

    // Check for movement input
    if (Input::IsKeyPressed(Key::Up))
    {
        target_direction = FORWARD_DIRECTION;

        if (Input::IsKeyPressed(Key::RightShift))
        {
            target_speed = RUN_SPEED;        // Running forward
            move_increment_speed = RUN_INCREMENT_SPEED;
        }
        else
        {
            target_speed = WALK_FORWARD_SPEED;      // Walking forward
        }
    }
    else if (Input::IsKeyPressed(Key::Down))
    {

        if (Input::IsKeyPressed(Key::RightShift))
        {
            target_direction = RUN_BACKWARD_DIRECTION;

            target_speed = RUN_SPEED;        // Running backward
            move_increment_speed = RUN_INCREMENT_SPEED;
        }
        else
        {
            target_direction = WALK_BACKWARD_DIRECTION;
            target_speed = WALK_BACKWARD_SPEED;      // Walking backward
        }
    }
    else if (Input::IsKeyPressed(Key::Left))
    {
        if (Input::IsKeyPressed(Key::RightShift))
        {
            target_speed = RUN_SPEED;        // Running backward
            move_increment_speed = RUN_INCREMENT_SPEED;
        }
        else
        {
            target_speed = WALK_FORWARD_SPEED;      // Walking backward
        }
    }
    else
    {
        // No movement input, decelerate to idle
        target_speed = IDLE_SPEED;
    }

    // Smoothly adjust direction (X-axis)
    if (target_direction != raptoid.blending_position.x)
    {
        if (target_direction > raptoid.blending_position.x)
        {
            raptoid.blending_position.x += delta_time * direction_increment_speed;
            if (raptoid.blending_position.x > target_direction)
                raptoid.blending_position.x = target_direction;
        }
        else
        {
            raptoid.blending_position.x -= delta_time * direction_increment_speed;
            if (raptoid.blending_position.x < target_direction)
                raptoid.blending_position.x = target_direction;
        }
    }

    // Smoothly adjust speed (Y-axis)
    if (target_speed != raptoid.blending_position.y)
    {
        if (target_speed > raptoid.blending_position.y)
        {
            raptoid.blending_position.y += delta_time * move_increment_speed;
            if (raptoid.blending_position.y > target_speed)
                raptoid.blending_position.y = target_speed;
        }
        else
        {
            raptoid.blending_position.y -= delta_time * DECREMENT_SPEED;
            if (raptoid.blending_position.y < target_speed)
                raptoid.blending_position.y = target_speed;
        }
    }

    RenderCommand::Clear();
    const glm::vec2 &delta = Input::GetMouseClickDragDelta();

    camera.OnMouseMove(delta);
    camera.OnUpdate(delta_time);

    camera.UpdateView();
    camera.UpdateProjection();

    cam_data.view_projection = camera.GetViewProjection();
    cam_data.positon = camera.GetPosition();

    ubo->Bind();
    ubo->SetData(&cam_data, sizeof(CamData));

    {
        shader->Enable();

        GLenum error = glGetError();
        OGN_CORE_ASSERT(error == GL_NO_ERROR, "[GL ERROR] {}", error);

        raptoid.blender.BlendAnimations(raptoid.blending_position, delta_time, animation_speed_playback);
        shader->SetBool("uhas_animation", raptoid.model->HasAnimations());
        shader->SetMatrix("ubone_transforms", raptoid.model->GetBoneTransforms()[0], raptoid.model->GetBoneTransforms().size());
        for (auto &mesh : raptoid.model->GetMeshes())
        {
            shader->SetMatrix("umodel_transform", mesh->transform);
            
            mesh->material.Bind();
            if (mesh->material.diffuse_texture)
            {
                mesh->material.diffuse_texture->Bind(DIFFUSE_TEXTURE_BINDING);
                shader->SetInt("udiffuse_texture", DIFFUSE_TEXTURE_BINDING);
            }
            if (mesh->material.specular_texture)
            {
                mesh->material.specular_texture->Bind(SPECULAR_TEXTURE_BINDING);
                shader->SetInt("uspecular_texture", SPECULAR_TEXTURE_BINDING);
            }
            if (mesh->material.roughness_texture)
            {
                mesh->material.roughness_texture->Bind(ROUGHNESS_TEXTURE_BINDING);
                shader->SetInt("uroughness_texture", ROUGHNESS_TEXTURE_BINDING);
            }
            RenderCommand::DrawIndexed(mesh->vertex_array);
            mesh->material.Unbind();
        }

        shader->Disable();
    }


    glDepthFunc(GL_LEQUAL);
    skybox_shader->Enable();

    GLenum error = glGetError();
    OGN_CORE_ASSERT(error == GL_NO_ERROR, "[GL ERROR] {}", error);

    skybox_shader->SetFloat("ublur_factor", blur_factor);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture_id);
    skybox_shader->SetInt("uskybox_cube", 0);

    RenderCommand::DrawIndexed(skybox->vao);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    skybox_shader->Disable();

    glDepthFunc(GL_LESS);
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
    ImGui::DragFloat("Animation Speed", &animation_speed_playback, 0.01f, 0.0f, 10.0f);
    ImGui::Separator();

    ImGui::SliderFloat("Direction", &raptoid.blending_position.x, raptoid.blender.GetMinSize().x, raptoid.blender.GetMaxSize().x);
    ImGui::SliderFloat("Speed", &raptoid.blending_position.y, raptoid.blender.GetMinSize().y, raptoid.blender.GetMaxSize().y);

    ImGui::SliderFloat("Blur Factor", &blur_factor, 0.0f, 1000.0f);

    ImGui::Separator();
    

    for (auto &state : raptoid.blender.GetStates())
    {
        ImGui::Text("%s\t%d: %.5f", state.name.c_str(), state.anim_index, state.weight);
    }

    ImGui::Separator();

    if (ImGui::Button("Play Sound"))
    {
        roar_sound->Play();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload Shader"))
    {
        shader->Reload();
        skybox_shader->Reload();
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
    // roar_sound->Play();
}
