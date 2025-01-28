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
const f32 SUN_DISTANCE = 10.0f;
i32 cascade_index = 0;
f32 sun_yaw = 180.0f;
f32 sun_pitch = 20.0f;
f32 padding = 5.0f; // Adjust this value based on your needs


SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
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

    light_center = {0.0f, 0.0f, 0.0f};
}

void SandboxLayer::OnAttach()
{
    Physics::Init(PhysicsAPI::Jolt);
    InitSounds();

    shader = Renderer::GetShader("SkinnedMesh");
    skybox_shader = Renderer::GetShader("Skybox");
    csm.shader = Renderer::GetShader("CascadedDepthMap");
    debug_r.shader = Shader::Create("Resources/Shaders/DepthMapDebug.glsl", false, false);

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
    InitCascadeShadow();
    InitDebugRenderer();
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
    RenderShadowMap();
    RenderDebug(delta_time);
    
    dir_light->Bind();
    RenderScene(delta_time);
}

void SandboxLayer::RenderDebug(f32 delta_time)
{
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // Clear to green
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    debug_r.shader->Enable();
    debug_r.framebuffer->Bind();
    debug_r.shader->SetInt("udepth_map", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, csm.framebuffer->GetDepthAttachment());
    debug_r.shader->SetInt("ucascade_index", cascade_index);
    glBindVertexArray(debug_r.vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    debug_r.shader->Disable();
}

void SandboxLayer::RenderShadowMap()
{
    glm::vec3 light_dir = glm::normalize(glm::vec3(
        std::cos(glm::radians(sun_pitch)) * std::sin(glm::radians(sun_yaw)),
        std::sin(glm::radians(sun_pitch)),
        std::cos(glm::radians(sun_pitch)) * std::cos(glm::radians(sun_yaw))
    ));
    
    glm::vec3 normalized_light_dir = glm::normalize(-light_dir);
    glm::vec3 light_position = normalized_light_dir * SUN_DISTANCE;
        
    glm::vec3 right = glm::cross(normalized_light_dir, glm::vec3(0.0f, 1.0f, 0.0f));
    if (glm::length(right) < 0.001f) {
        right = glm::cross(normalized_light_dir, glm::vec3(1.0f, 0.0f, 0.0f));
    }
    right = glm::normalize(right);
    glm::vec3 up = glm::normalize(glm::cross(right, normalized_light_dir));
        
    // Create view matrix
    glm::mat4 light_view = glm::lookAt(light_position, normalized_light_dir, up);
    
    UpdateLightSpaceMatrices(light_position, light_view, 45.0f, 1280.0f / 720.0f, 0.1f);
    csm.shader->Enable();
    glViewport(0, 0, csm.framebuffer->GetWidth(), csm.framebuffer->GetHeight());
    for (i32 i = 0; i < NUM_CASCADES; ++i)
    {
        csm.framebuffer->Bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            csm.framebuffer->GetDepthAttachment(), 0, i);

        OGN_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
            "Framebuffer incomplete for cascade");
        
        csm.shader->SetMatrix("ulight_matrix", light_space_matrices[i]);
        csm.shader->SetBool("uhas_animation", floor_model.model->HasAnimations());
        csm.shader->SetMatrix("umodel_transform", glm::scale(glm::mat4(1.0f), glm::vec3(30.0f)));
        for (auto &mesh : floor_model.model->GetMeshes())
        {
            RenderCommand::DrawIndexed(mesh->vertex_array);
        }
        csm.shader->SetBool("uhas_animation", character_model.model->HasAnimations());
        csm.shader->SetMatrix("ubone_transforms", character_model.model->GetBoneTransforms()[0], character_model.model->GetBoneTransforms().size());
        csm.shader->SetMatrix("umodel_transform", glm::scale(glm::mat4(1.0f), glm::vec3(20.0f)));
        for (auto &mesh : character_model.model->GetMeshes())
        {
            RenderCommand::DrawIndexed(mesh->vertex_array);
        }
    }
    csm.framebuffer->Unbind();
    
    csm.shader->Disable();
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

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D_ARRAY, csm.framebuffer->GetDepthAttachment());
    shader->SetInt("ushadow_map", 3);

    shader->SetFloat("ucascade_plane_distances", cascade_splits.data(), cascade_splits.size());
    shader->SetFloat("ushadow_bias", csm.shadow_bias.data(), csm.shadow_bias.size());
    shader->SetMatrix("ulight_matrices", light_space_matrices[0], light_space_matrices.size());

    // floor
    shader->SetBool("uhas_animation", floor_model.model->HasAnimations());
    shader->SetMatrix("umodel_transform", glm::scale(glm::mat4(1.0f), glm::vec3(30.0f)));

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
    shader->SetMatrix("ubone_transforms", character_model.model->GetBoneTransforms()[0], character_model.model->GetBoneTransforms().size());
    shader->SetMatrix("umodel_transform", glm::scale(glm::mat4(1.0f), glm::vec3(20.0f)));

    for (auto &mesh : character_model.model->GetMeshes())
    {
        MaterialManager::UpdateMaterial(mesh->material_index, mesh->material.buffer_data);
        shader->SetInt("umaterial_index", mesh->material_index);
        mesh->material.Update(shader.get());
        RenderCommand::DrawIndexed(mesh->vertex_array);
    }

    shader->Disable();

    RenderSkybox(delta_time);

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

    ImGui::DragFloat("Padding", &padding, 1.0f, 0.0f, 200.0f);

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

    ImGui::Separator();
    for (auto &state : character_model.blender.GetStates())
    {
        ImGui::Text("%s\t%d: %.5f", state.name.c_str(), state.anim_index, state.weight);
    }
    
    ImGui::SliderInt("Cascade Index", &cascade_index, 0, NUM_CASCADES - 1);
    for (i32 i = 0; i < NUM_CASCADES; ++i)
    {
        ImGui::Text("cascade %d: %.2f",i, cascade_splits[i]);
    }

    ImGui::Separator();

    if (ImGui::Button("Play Sound"))
    {
        roar_sound->Play();
    }
    ImGui::SameLine();

    if (ImGui::Button("Reload Skinning Shader"))
    {
        shader->Reload();
    }

    ImGui::End();

    ImGui::Begin("Cascade Shadows");
    ImGui::Text("Camera distance %.2f", glm::length(camera.GetPosition()));

    std::string cascade_label = "Cascade";
    if (ImGui::TreeNodeEx(cascade_label.c_str(), ImGuiTreeNodeFlags_DefaultOpen, cascade_label.c_str()))
    {
        ImGui::Image((void *)(intptr_t)debug_r.framebuffer->GetColorAttachment(0),
            ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().x / 16.0f * 9.0f), ImVec2(0, 1), ImVec2(1, 0));
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
    CleanupFramebuffers();
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

std::vector<f32> SandboxLayer::CalculateCascadeSplits(f32 near_plane, f32 far_plane)
{
    std::vector<f32> splits(NUM_CASCADES);
    f32 lambda = 0.5; // balance between uniform and logarithmic splitting
    f32 ratio = far_plane / near_plane;

    for (i32 i = 0; i < NUM_CASCADES; ++i)
    {
        f32 p = (i + 1) / static_cast<f32>(NUM_CASCADES);
        f32 log = near_plane * std::pow(ratio, p);
        f32 uniform = near_plane + (far_plane - near_plane) * p;
        f32 d = lambda * (log - uniform) + uniform;
        splits[i] = d;
    }

    return splits;
}

void SandboxLayer::UpdateLightSpaceMatrices(const glm::vec3 &light_dir, const glm::mat4 &light_view, f32 fov, f32 aspect, f32 near_plane)
{
    light_space_matrices.clear();

    for (int i = 0; i < NUM_CASCADES; ++i) {
        float prev_split = (i == 0) ? near_plane : cascade_splits[i - 1];
        float current_split = cascade_splits[i];

        // Get frustum corners in world space
        std::vector<glm::vec4> frustum_corners = GetFrustumCorners(fov, aspect, prev_split, current_split);

        // Transform corners to light space
        glm::vec3 min_corner(std::numeric_limits<float>::max());
        glm::vec3 max_corner(std::numeric_limits<float>::lowest());  // Note: Changed from min()

        for (const auto& corner : frustum_corners) {
            glm::vec4 light_space_corner = light_view * corner;
            min_corner = glm::min(min_corner, glm::vec3(light_space_corner));
            max_corner = glm::max(max_corner, glm::vec3(light_space_corner));
        }

        min_corner -= glm::vec3(padding);
        max_corner += glm::vec3(padding);

        // Create the light projection matrix
        glm::mat4 light_projection = glm::ortho(
            min_corner.x, max_corner.x,
            min_corner.y, max_corner.y,
            min_corner.z, max_corner.z
        );

        // Store the combined matrix
        light_space_matrices.push_back(light_projection * light_view);
    }
}

std::vector<glm::vec4> SandboxLayer::GetFrustumCorners(f32 fov, f32 aspect, f32 near_plane, f32 far_plane)
{
    std::vector<glm::vec4> corners;
    corners.reserve(8);
        
    float tan_half_fov = std::tan(glm::radians(fov * 0.5f));
        
    for (int x = 0; x < 2; ++x) {
        for (int y = 0; y < 2; ++y) {
            for (int z = 0; z < 2; ++z) {
                float z_value = (z == 0) ? near_plane : far_plane;
                glm::vec4 corner(
                    (x == 0 ? -1.0f : 1.0f) * z_value * tan_half_fov * aspect,
                    (y == 0 ? -1.0f : 1.0f) * z_value * tan_half_fov,
                    -z_value,
                    1.0f
                );
                corners.push_back(corner);
            }
        }
    }
        
    return corners;
}

void SandboxLayer::InitCascadeShadow()
{
    cascade_splits = CalculateCascadeSplits(10.0f, 100.0f);
    csm.shadow_bias.resize(NUM_CASCADES);
    csm.shadow_bias[0] = 0.000025f;
    csm.shadow_bias[1] = 0.000075f;
    csm.shadow_bias[2] = 0.00025f; 
    csm.shadow_bias[3] = 0.0005f;

    FramebufferSpecification spec;
    spec.attachments = {
        FramebufferTextureFormat::DEPTH24STENCIL8_ARRAY
    };
    spec.width = 1280;
    spec.height = 720;
    spec.depth_array_count = NUM_CASCADES;
    csm.framebuffer = Framebuffer::Create(spec);
}

void SandboxLayer::InitDebugRenderer()
{
    FramebufferSpecification spec;
    spec.attachments = {
        FramebufferTextureFormat::RGBA8
    };
    spec.width = 1280;
    spec.height = 720;
    debug_r.framebuffer = Framebuffer::Create(spec);
    
    u32 quad_vbo;
    f32 quad_vertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };

    glGenVertexArrays(1, &debug_r.vao);
    glGenBuffers(1, &quad_vbo);
    glBindVertexArray(debug_r.vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
}

void SandboxLayer::CleanupFramebuffers()
{
}

