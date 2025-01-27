// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Core/Input.h"
#include "Origin/GUI/UI.h"
#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/FmodSound.h"
#include "Origin/Audio/FmodDsp.h"
#include "SandboxLayer.h"
#include <glad/glad.h>

using namespace origin;

struct CamData
{
    glm::mat4 view_projection;
    glm::vec3 positon;
};

CamData cam_data;
Ref<UniformBuffer> ubo;

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
    explicit Data(const std::string &filepath)
    {
        model = Model::Create(filepath);
        anims = model->GetAnimations();
        blender.SetModel(model);
    }
};

Data character, floor_;
f32 animation_speed_playback = 1.0f;
f32 increment_speed = 300.0f;
f32 decrement_speed = 800.0f;
f32 target_direction = 180.0f;
f32 target_speed = 0.0f;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    camera.SetPosition({ -1.62620163f, 1.37793064f, 2.60992050f });
    camera.SetPitch(0.161835521f);
    camera.SetYaw(0.640095532f);

    camera2.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    camera2.SetPosition({ 0.0f, 2.0f, 10.0f });
    
    RenderCommand::ClearColor({ 0.125f, 0.125f, 0.125f, 1.0f });
}

void SandboxLayer::OnAttach()
{
    Physics::Init(PhysicsAPI::Jolt);
    InitSounds();

    shader = Renderer::GetShader("SkinnedMesh");
    skybox_shader = Renderer::GetShader("Skybox");
    shadow_map_shader = Renderer::GetShader("ShadowMap");

    ubo = UniformBuffer::Create(sizeof(CamData), 0);

    floor_ = Data("Resources/Models/checker_plane.glb");
    character = Data("Resources/Models/base_character.glb");
    character.blender.SetRange({ 0.0f, 0.0f }, { 360.0f, 400.0f });
    // character animation setup
    {
        character.blender.AddAnimation("Idling", { 0.0f, 0.0f }, { 72.0f, 25.0f }); // idle
        character.blender.AddAnimation("Idling", { 72.0f, 0.0f }, { 144.0f, 25.0f }); // idle
        character.blender.AddAnimation("Idling", { 144.0f, 0.0f }, { 216.0f, 25.0f }); // idle
        character.blender.AddAnimation("Idling", { 216.0f, 0.0f }, { 288.0f, 25.0f }); // idle
        character.blender.AddAnimation("Idling", { 288.0f, 0.0f }, { 360.0f, 25.0f }); // idle

        character.blender.AddAnimation("Walking_Forward", { 72.0f, 50.0f }, { 180.0f, 150.0f }); // walking
        character.blender.AddAnimation("Walking_Forward", { 180.0f, 50.0f }, { 288.0f, 150.0f }); // walking

        character.blender.AddAnimation("Walking_Backward", { 0.0f, 50.0f }, { 72.0f, 150.0f }); // walking backward
        character.blender.AddAnimation("Running_Backward", { 0.0f, 200.0f }, { 120.0f, 400.0f }); // backward
        character.blender.AddAnimation("Running_Forward", { 144.0f, 180.0f }, { 216.0f, 400.0f }); // forward
        character.blender.AddAnimation("Running_Backward", { 230.0f, 200.0f }, { 360.0f, 400.0f }); // backward
        character.blender.AddAnimation("Walking_Backward", { 288.0f, 50.0f }, { 360.0f, 150.0f }); // walking backward
    }
    

    skybox = Skybox::Create("Resources/Skybox", ".jpg");

    // directional light
    dir_light = CreateRef<DirectionalLight>();
    dir_light->data.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    dir_light->data.direction = { -90.0f, -70.0f, 0.0f, 1.0f };
}

void SandboxLayer::OnUpdate(const Timestep delta_time)
{
    UpdateCharacterMovement(delta_time);
    UpdateCamera(delta_time);

    // directional light
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, Application::GetInstance().GetWindow().GetWidth(), Application::GetInstance().GetWindow().GetHeight());
    dir_light->Bind();

    RenderDebug(delta_time);

    RenderScene(delta_time);
    RenderSkybox(delta_time);
}

void SandboxLayer::RenderDebug(f32 delta_time)
{
    Renderer2D::Begin();
    Renderer2D::DrawQuad(glm::mat4(1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    for (const auto & [fst, snd] : camera2.GetFrustum().GetEdges())
    {
        Renderer2D::DrawLine(fst, snd, { 1.0f, 0.0f, 0.0f, 1.0f });
    }
    Renderer2D::End();
}

void SandboxLayer::UpdateCamera(f32 delta_time)
{
    const glm::vec2 &mouse_click_drag_delta = Input::GetMouseClickDragDelta();
    camera.OnMouseMove(mouse_click_drag_delta);
    camera.OnUpdate(delta_time);

    camera.UpdateView();
    camera.UpdateProjection();

    cam_data.view_projection = camera.GetViewProjection();
    cam_data.positon = camera.GetPosition();
    ubo->Bind();
    ubo->SetData(&cam_data, sizeof(CamData));
}

void SandboxLayer::RenderScene(f32 delta_time)
{
    shader->Enable();

    // floor
    shader->SetBool("uhas_animation", floor_.model->HasAnimations());
    shader->SetMatrix("umodel_transform", glm::scale(glm::mat4(1.0f), glm::vec3(10.0f)));

    for (auto &mesh : floor_.model->GetMeshes())
    {
        MaterialManager::UpdateMaterial(mesh->material_index, mesh->material.buffer_data);
        shader->SetInt("umaterial_index", mesh->material_index);
        mesh->material.Update(shader.get());
        RenderCommand::DrawIndexed(mesh->vertex_array);
    }

    // CHARACTER
    character.blender.BlendAnimations(character.blending_position, delta_time, animation_speed_playback);
    shader->SetBool("uhas_animation", character.model->HasAnimations());
    shader->SetMatrix("ubone_transforms", character.model->GetBoneTransforms()[0], character.model->GetBoneTransforms().size());
    shader->SetMatrix("umodel_transform", glm::mat4(1.0));

    for (auto &mesh : character.model->GetMeshes())
    {
        MaterialManager::UpdateMaterial(mesh->material_index, mesh->material.buffer_data);
        shader->SetInt("umaterial_index", mesh->material_index);
        mesh->material.Update(shader.get());
        RenderCommand::DrawIndexed(mesh->vertex_array);
    }

    shader->Disable();
}

void SandboxLayer::RenderSkybox(f32 delta_time)
{
    glDepthFunc(GL_LEQUAL);

    Renderer::GetShader("Skybox")->Enable();
    Renderer::skybox_uniform_buffer->Bind();
    Renderer::skybox_uniform_buffer->SetData(&tint_color, sizeof(glm::vec4) + sizeof(f32));
    Renderer::skybox_uniform_buffer->SetData(&blur_factor, sizeof(glm::vec4) + sizeof(f32), sizeof(glm::vec4));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture_id);
    Renderer::GetShader("Skybox")->SetInt("uskybox_cube", 0);

    RenderCommand::DrawIndexed(skybox->vao);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    Renderer::GetShader("Skybox")->Disable();

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
    ImGui::DragFloat("Animation Speed", &animation_speed_playback, 0.01f, 0.0f, 10.0f);

    ImGui::Separator();
    ImGui::SliderFloat("Direction", &character.blending_position.x, character.blender.GetMinSize().x, character.blender.GetMaxSize().x);
    ImGui::SliderFloat("Speed", &character.blending_position.y, character.blender.GetMinSize().y, character.blender.GetMaxSize().y);

    ImGui::Separator();
    ImGui::ColorEdit4("Tint Color", glm::value_ptr(tint_color));
    ImGui::SliderFloat("Blur Factor", &blur_factor, 0.0f, 1.0f);

    ImGui::Separator();

    glm::vec3 light_direction = glm::vec3(dir_light->data.direction);
    if (UI::DrawVec3Control("Light Direction", light_direction, 0.1f, 0.0f))
    {
        dir_light->data.direction.x = light_direction.x;
        dir_light->data.direction.y = light_direction.y;
        dir_light->data.direction.z = light_direction.z;
    }

    ImGui::ColorEdit4("Light Color", glm::value_ptr(dir_light->data.color));

    ImGui::Separator();
    for (auto &state : character.blender.GetStates())
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

#if 0
    for (int i = 0; i < NUM_CASCADES; ++i)
    {
        std::string cascade_label = "Cascade " + std::to_string(i);
        if (ImGui::TreeNode(cascade_label.c_str()))
        {
            ImGui::Text("Cascade %d", i);

            GLuint texture_layer;
            glGenTextures(1, &texture_layer);
            glBindTexture(GL_TEXTURE_2D, texture_layer);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Copy layer from shadow map texture array
            glCopyImageSubData(
                shadow_map_texture, GL_TEXTURE_2D_ARRAY, 0, 0, 0, i,
                texture_layer, GL_TEXTURE_2D, 0, 0, 0, 0,
                shadow_map_resolution, shadow_map_resolution, 1
            );

            ImGui::Image((void *)(intptr_t)texture_layer, ImVec2(256, 256));

            // Cleanup
            glDeleteTextures(1, &texture_layer);

            ImGui::TreePop();
        }
    }
#endif

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

void SandboxLayer::UpdateCharacterMovement(f32 delta_time)
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
    f32 target_direction = character.blending_position.x;
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
    if (target_direction != character.blending_position.x)
    {
        if (target_direction > character.blending_position.x)
        {
            character.blending_position.x += delta_time * direction_increment_speed;
            if (character.blending_position.x > target_direction)
                character.blending_position.x = target_direction;
        }
        else
        {
            character.blending_position.x -= delta_time * direction_increment_speed;
            if (character.blending_position.x < target_direction)
                character.blending_position.x = target_direction;
        }
    }

    // Smoothly adjust speed (Y-axis)
    if (target_speed != character.blending_position.y)
    {
        if (target_speed > character.blending_position.y)
        {
            character.blending_position.y += delta_time * move_increment_speed;
            if (character.blending_position.y > target_speed)
                character.blending_position.y = target_speed;
        }
        else
        {
            character.blending_position.y -= delta_time * DECREMENT_SPEED;
            if (character.blending_position.y < target_speed)
                character.blending_position.y = target_speed;
        }
    }
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
