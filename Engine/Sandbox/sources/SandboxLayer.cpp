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

Data character_model, floor_model;
f32 animation_speed_playback = 1.0f;
f32 increment_speed = 300.0f;
f32 decrement_speed = 800.0f;
f32 target_direction = 180.0f;
f32 target_speed = 0.0f;
i32 cascade_index = 0;
f32 sun_yaw = 230.0f;
f32 sun_pitch = 210.0f;

glm::mat4 floor_scale = glm::scale(glm::mat4(1.0f), {20.0f, 1.0f, 20.0f});
glm::mat4 character_transform = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    // fov, aspect ratio, near, far
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 1.0f, 700.0f);
    camera.SetPosition({ -1.62620163f, 1.37793064f, 2.60992050f });
    camera.SetPitch(0.161835521f);
    camera.SetYaw(0.640095532f);
    camera.UpdateView();
    camera.UpdateProjection();

    RenderCommand::ClearColor({ 0.125f, 0.125f, 0.125f, 1.0f });

    FramebufferSpecification fbSpec;
    fbSpec.attachments =
    {
        FramebufferTextureFormat::RGBA8,
        FramebufferTextureFormat::DEPTH24STENCIL8
    };

    fbSpec.width = 1280;
    fbSpec.height = 720;
    main_framebuffer = Framebuffer::Create(fbSpec);
}

void SandboxLayer::OnAttach()
{
    Physics::Init(PhysicsAPI::Jolt);
    InitSounds();

    shader = Renderer::GetShader("SkinnedMesh");
    skybox_shader = Renderer::GetShader("Skybox");
    ubo = UniformBuffer::Create(sizeof(CamData), 0);

    floor_model = Data("Resources/Models/checker_plane.glb");
    character_model = Data("Resources/Models/base_character.glb");
    character_model.blender.SetRange({ 0.0f, 0.0f }, { 360.0f, 400.0f });
    // character animation setup
    {
        character_model.blender.AddAnimation("Idling", { 0.0f, 0.0f }, { 72.0f, 25.0f }); // idle
        character_model.blender.AddAnimation("Idling", { 72.0f, 0.0f }, { 144.0f, 25.0f }); // idle
        character_model.blender.AddAnimation("Idling", { 144.0f, 0.0f }, { 216.0f, 25.0f }); // idle
        character_model.blender.AddAnimation("Idling", { 216.0f, 0.0f }, { 288.0f, 25.0f }); // idle
        character_model.blender.AddAnimation("Idling", { 288.0f, 0.0f }, { 360.0f, 25.0f }); // idle

        character_model.blender.AddAnimation("Walking_Forward", { 72.0f, 50.0f }, { 180.0f, 150.0f }); // walking
        character_model.blender.AddAnimation("Walking_Forward", { 180.0f, 50.0f }, { 288.0f, 150.0f }); // walking

        character_model.blender.AddAnimation("Walking_Backward", { 0.0f, 50.0f }, { 72.0f, 150.0f }); // walking backward
        character_model.blender.AddAnimation("Running_Backward", { 0.0f, 200.0f }, { 120.0f, 400.0f }); // backward
        character_model.blender.AddAnimation("Running_Forward", { 144.0f, 180.0f }, { 216.0f, 400.0f }); // forward
        character_model.blender.AddAnimation("Running_Backward", { 230.0f, 200.0f }, { 360.0f, 400.0f }); // backward
        character_model.blender.AddAnimation("Walking_Backward", { 288.0f, 50.0f }, { 360.0f, 150.0f }); // walking backward
    }

    skybox = Skybox::Create("Resources/Skybox", ".jpg");

    // directional light
    dir_light = CreateRef<DirectionalLight>();
    dir_light->data.color = { 1.0f, 1.0f, 1.0f, 1.0f };
    dir_light->data.direction = { 20.0f, -20.0f, 0.0f, 1.0f };
}

void SandboxLayer::OnUpdate(const Timestep delta_time)
{
    if (const auto &fb_spec = main_framebuffer->GetSpecification(); viewport_size.x != fb_spec.width || viewport_size.y != fb_spec.height)
    {
        if (viewport_size.x > 0.0f && viewport_size.y > 0.0f)
        {
            main_framebuffer->Resize(static_cast<u32>(viewport_size.x), static_cast<u32>(viewport_size.y));
            camera.SetViewportSize(main_framebuffer->GetWidth(), main_framebuffer->GetHeight());
        }
    }

    UpdateCharacterMovement(delta_time);
    if (viewport_hovered) UpdateCamera(delta_time);

    cam_data.view_projection = camera.GetViewProjection();
    cam_data.positon = camera.GetPosition();
    ubo->Bind();
    ubo->SetData(&cam_data, sizeof(CamData));
    dir_light->Bind();
    RenderScene(delta_time);
}

void SandboxLayer::UpdateCamera(f32 delta_time)
{
    const glm::vec2 &mouse_click_drag_delta = Input::GetMouseClickDragDelta();
    camera.OnMouseMove(mouse_click_drag_delta);
    camera.OnUpdate(delta_time);

    camera.UpdateView();
    camera.UpdateProjection();
}

void SandboxLayer::RenderScene(f32 delta_time)
{
    main_framebuffer->Bind();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    shader->Enable();

    // floor
    shader->SetBool("uhas_animation", floor_model.model->HasAnimations());
    shader->SetMatrix("umodel_transform", floor_scale);

    for (auto &mesh : floor_model.model->GetMeshes())
    {
        MaterialManager::UpdateMaterial(mesh->material_index, mesh->material.buffer_data);
        shader->SetInt("umaterial_index", mesh->material_index);
        mesh->material.Update(shader.get());
        RenderCommand::DrawIndexed(mesh->vertex_array);
    }

    // CHARACTER
    character_model.blender.BlendAnimations(character_model.blending_position, delta_time, animation_speed_playback);
    shader->SetBool("uhas_animation", character_model.model->HasAnimations());
    shader->SetMatrix("ubone_transforms", character_model.model->GetBoneTransforms()[0], 
        (i32)character_model.model->GetBoneTransforms().size());
    shader->SetMatrix("umodel_transform", character_transform);

    for (auto &mesh : character_model.model->GetMeshes())
    {
        MaterialManager::UpdateMaterial(mesh->material_index, mesh->material.buffer_data);
        shader->SetInt("umaterial_index", mesh->material_index);
        mesh->material.Update(shader.get());
        RenderCommand::DrawIndexed(mesh->vertex_array);
    }

    shader->Disable();

    RenderSkybox(delta_time);

    // render infinity grid
    glEnable(GL_BLEND);
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    Renderer::GetShader("GridShader")->Enable();

    glBindVertexArray(m_GridVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    Renderer::GetShader("GridShader")->Disable();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDepthFunc(GL_LESS);

    main_framebuffer->Unbind();
}

void SandboxLayer::RenderSkybox(f32 delta_time)
{
    glDepthFunc(GL_LEQUAL);

    Renderer::GetShader("Skybox")->Enable();
    Renderer::skybox_uniform_buffer->Bind();
    Renderer::skybox_uniform_buffer->SetData(&tint_color, sizeof(glm::vec4) + sizeof(f32));
    Renderer::skybox_uniform_buffer->SetData(&blur_factor, sizeof(glm::vec4) + sizeof(f32), sizeof(glm::vec4));
    Renderer::GetShader("Skybox")->SetInt("uskybox_cube", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture_id);

    RenderCommand::DrawIndexed(skybox->vao);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    Renderer::GetShader("Skybox")->Disable();

    glDepthFunc(GL_LESS);
}

void SandboxLayer::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<FramebufferResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
    dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseScroll));

    camera.OnEvent(e);
}

void SandboxLayer::OnGuiRender()
{
    Dockspace::Begin();
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;
    ImGui::Begin("Scene", nullptr, window_flags);
    viewport_hovered = ImGui::IsWindowHovered();
    
    viewport_size.x = ImGui::GetWindowSize().x;
    viewport_size.y = ImGui::GetWindowSize().y;

    ImGui::Image(reinterpret_cast<void*>((uintptr_t)main_framebuffer->GetColorAttachment(0)),
        ImVec2(viewport_size.x, viewport_size.y), ImVec2(0, 1), ImVec2(1, 0));
    
    ImGui::End();
    
    ImGui::Begin("Control");

    const f32 &fps = ImGui::GetIO().Framerate;
    ImGui::Text("%.2f", fps);
    ImGui::DragFloat("Animation Speed", &animation_speed_playback, 0.01f, 0.0f, 10.0f);

    ImGui::Separator();
    ImGui::SliderFloat("Direction", &character_model.blending_position.x, character_model.blender.GetMinSize().x, character_model.blender.GetMaxSize().x);
    ImGui::SliderFloat("Speed", &character_model.blending_position.y, character_model.blender.GetMinSize().y, character_model.blender.GetMaxSize().y);

    ImGui::Separator();
    ImGui::ColorEdit4("Tint Color", glm::value_ptr(tint_color));
    ImGui::SliderFloat("Blur Factor", &blur_factor, 0.0f, 1.0f);

    ImGui::Separator();

    bool dragged = ImGui::DragFloat("Sun Yaw", &sun_yaw, 1.0f, 0.0f, 360.0f);
    dragged |= ImGui::DragFloat("Sun Pitch", &sun_pitch, 1.0f, 0.0f, 360.0f);
    if (dragged)
    {
        glm::vec3 light_direction(
            cos(glm::radians(sun_pitch)) * sin(glm::radians(sun_yaw)),
            sin(glm::radians(sun_pitch)),
            cos(glm::radians(sun_pitch)) * cos(glm::radians(sun_yaw))
        );
    
        light_direction = glm::normalize(light_direction);
        dir_light->data.direction.x = light_direction.x;
        dir_light->data.direction.y = light_direction.y;
        dir_light->data.direction.z = light_direction.z;
    }

    ImGui::ColorEdit4("Light Color", glm::value_ptr(dir_light->data.color));


    if (ImGui::TreeNode("Animation States"))
    {
        for (auto &state : character_model.blender.GetStates())
        {
            ImGui::Text("%s\t%d: %.5f", state.name.c_str(), state.anim_index, state.weight);
        }
        ImGui::TreePop();
    }
    
    
    ImGui::Separator();

    if (ImGui::Button("Play Sound"))
    {
        roar_sound->Play();
    }

    if (ImGui::TreeNode("Shader"))
    {
        if (ImGui::Button("Reload Skinning Shader"))
        {
            shader->Reload();
        }

        if (ImGui::Button("Reload Grid Shader"))
        {
            Renderer::GetShader("GridShader")->Reload();
        }
        ImGui::TreePop();
    }
    ImGui::End();
    Dockspace::End();
}

bool SandboxLayer::OnWindowResize(FramebufferResizeEvent &e)
{
    RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    return false;
}

bool SandboxLayer::OnMouseScroll(MouseScrolledEvent &e)
{
    if (viewport_hovered)
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
    f32 target_direction = character_model.blending_position.x;
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
    if (target_direction != character_model.blending_position.x)
    {
        if (target_direction > character_model.blending_position.x)
        {
            character_model.blending_position.x += delta_time * direction_increment_speed;
            if (character_model.blending_position.x > target_direction)
                character_model.blending_position.x = target_direction;
        }
        else
        {
            character_model.blending_position.x -= delta_time * direction_increment_speed;
            if (character_model.blending_position.x < target_direction)
                character_model.blending_position.x = target_direction;
        }
    }

    // Smoothly adjust speed (Y-axis)
    if (target_speed != character_model.blending_position.y)
    {
        if (target_speed > character_model.blending_position.y)
        {
            character_model.blending_position.y += delta_time * move_increment_speed;
            if (character_model.blending_position.y > target_speed)
                character_model.blending_position.y = target_speed;
        }
        else
        {
            character_model.blending_position.y -= delta_time * DECREMENT_SPEED;
            if (character_model.blending_position.y < target_speed)
                character_model.blending_position.y = target_speed;
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