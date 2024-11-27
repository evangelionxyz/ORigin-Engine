// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/GUI/UI.h"

#include "SandboxLayer.h"

using namespace origin;

struct Data
{
    Model model;
    std::vector<ModelAnimation> animations;
    Animator animator;

    glm::vec3 light_pos = { 0.0f, 10.0f, -3.0f };
    glm::vec3 light_color = { 1.0f, 1.0f, 1.0f };
};

static Data s_data;
Ref<Shader> shader;

void draw_mesh(const glm::mat4 &view_projection, const glm::mat4 &transform, const Ref<VertexArray> &va)
{
    shader->SetMatrix("viewProjection", view_projection * transform);
    shader->SetMatrix("model_transform", transform);
    RenderCommand::DrawIndexed(va);
}

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    camera.SetPosition({ 0.0f, 0.0f, 10.0f });
    camera.SetAllowedMove(true);

    shader = Shader::Create("Resources/Shaders/TestShader.glsl", false, true);
    s_data.model = Model("Resources/Models/kay_kit/Characters/gltf/Knight.glb");
    // s_data.model = Model("Resources/Models/storm_trooper/sss.glb");

    s_data.animations = s_data.model.GetAnimations();
    s_data.animator = Animator(&s_data.animations[0]);
    s_data.animator.PlayAnimation(&s_data.animations[0]);

    RenderCommand::ClearColor({ 0.8f,0.8f,0.8f, 1.0f });

    Application::GetInstance().GetWindow().ToggleVSync();
}

void SandboxLayer::OnAttach()
{
}

void SandboxLayer::OnUpdate(const Timestep ts)
{
    RenderCommand::Clear();
    camera.OnUpdate(ts);

    // s_data.animator.UpdateAnimation(ts, 1.0f);
    // s_data.animator.ApplyToMeshes();

    MeshRenderer::Begin(camera);
    for (auto &m : s_data.model.GetMeshes())
    {
        if (!m->is_active)
            continue;

        shader->Enable();
        for (auto t : m->material.textures)
        {
            if (t.contains(aiTextureType_DIFFUSE))
            {
                t.at(aiTextureType_DIFFUSE)->Bind(0);
                shader->SetInt("texture_diffuse", 0);
            }
           /* else if (t.contains(aiTextureType_SPECULAR))
            {
                t.at(aiTextureType_SPECULAR)->Bind(1);
                shader->SetInt("texture_specular", 1);
            }
            else if (t.contains(aiTextureType_NORMALS))
            {
                t.at(aiTextureType_NORMALS)->Bind(2);
                shader->SetInt("texture_normals", 2);
            }
            else if (t.contains(aiTextureType_BASE_COLOR))
            {
                t.at(aiTextureType_BASE_COLOR)->Bind(3);
                shader->SetInt("texture_base_color", 3);
            }*/
        }

        shader->SetVector("lightPosition", s_data.light_pos);
        shader->SetVector("viewPosition", camera.GetPosition());
        shader->SetVector("lightColor", s_data.light_color);
        shader->SetFloat("shininess", 1.0f);
        shader->SetMatrix("bone_transforms", m->final_bone_matrices[0], m->final_bone_matrices.size());
        draw_mesh(camera.GetViewProjection(), m->transform, m->vertex_array);
        shader->Disable();
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

    ImGui::Begin("Control");
    ImGui::DragFloat3("Light Pos", &s_data.light_pos.x);
    ImGui::ColorEdit3("Light Color", &s_data.light_color.x);
    ImGui::End();

    ImGui::Begin("Test");

    for (auto &m : s_data.model.GetMeshes())
    {
        ImGui::PushID(m->name.c_str());
        ImGui::Checkbox(m->name.c_str(), &m->is_active);
        ImGui::Text("bone count: %d", m->bone_count);
        for (auto tvector : m->material.textures)
        {
            for (auto tmap : tvector)
            {
                ImTextureID tex_id = reinterpret_cast<ImTextureID>((uintptr_t)tmap.second->GetID());
                ImGui::Image(tex_id, ImVec2(256, 128));
            }
        }
        ImGui::PopID();
    }
    ImGui::End();

    ImGui::Begin("Animations");
    for (auto &a : s_data.animations)
    {
        if (ImGui::Button(a.name.c_str()))
        {
            s_data.animator.PlayAnimation(&a);
        }
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
