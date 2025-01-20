// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Core/Input.h"
#include "Origin/GUI/UI.h"

#include "SandboxLayer.h"


using namespace origin;

struct Data
{
    Model model;
    glm::vec3 position {0.0f, 0.0f, 0.0f};
    std::vector<ModelAnimation> animations;
    Animator animator;

    glm::vec3 light_pos = { -67.0f, 10.0f, 14.0f };
    glm::vec3 light_color = { 0.65f, 0.65f, 0.65f };
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
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 10000.0f);
    camera.SetPosition({ 0.0f, 2.0f, 8.0f });

    shader = Shader::Create("Resources/Shaders/TestShader.glsl", false, true);
    s_data.model = Model("Resources/Models/kay_kit/Characters/gltf/Knight.glb");
    // s_data.model = Model("Resources/Models/Test/Test.glb");
    // s_data.model = Model("Resources/Models/storm_trooper/sss.glb");
    // s_data.model = Model("Resources/Models/cube_plane.glb");
    // s_data.model = Model("Resources/Models/survival_guitar_backpack.glb");
    // s_data.model = Model("Resources/Models/raptoid.glb");

    s_data.animations = s_data.model.GetAnimations();
    if (!s_data.animations.empty())
    {
        s_data.animator = Animator(&s_data.animations[0]);
        s_data.animator.PlayAnimation(&s_data.animations[0]);
    }

    RenderCommand::ClearColor({ 0.3f,0.3f,0.3f, 1.0f });

    //Application::GetInstance().GetWindow().ToggleVSync();
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

    s_data.animator.UpdateAnimation(ts, 1.0f);
    s_data.animator.ApplyToMeshes();

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
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnKeyPressed));
    dispatcher.Dispatch<MouseMovedEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseMove));
    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseButtonPressed));
    dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseScroll));

    camera.OnEvent(e);
}

void SandboxLayer::OnGuiRender()
{
    ImGui::Begin("Control");
    ImGui::DragFloat3("Light Pos", &s_data.light_pos.x);
    ImGui::ColorEdit3("Light Color", &s_data.light_color.x);
    if (ImGui::Button("Reload shader"))
    {
        shader->Reload();
    }
    ImGui::End();

    ImGui::Begin("Test");

    for (auto &m : s_data.model.GetMeshes())
    {
        ImGui::PushID(m->name.c_str());
        ImGui::Checkbox(m->name.c_str(), &m->is_active);
        ImGui::Text("bone count: %d", m->bone_count);

        /*glm::vec3 pos, rot, scale;
        Math::DecomposeTransformEuler(m->transform, pos, rot, scale);

        ImGui::DragFloat3("Rotation", &rot.x);
        ImGui::DragFloat3("Scale", &scale.x);

        //m->transform = glm::recompose(scale, glm::quat(rot), pos, glm::vec3(0.0f), glm::vec4(1.0f));

        /* for (auto tvector : m->material.textures)
         {
             for (auto tmap : tvector)
             {
                 ImTextureID tex_id = reinterpret_cast<ImTextureID>((uintptr_t)tmap.second->GetID());
                 ImGui::Image(tex_id, ImVec2(256, 128));
             }
         }*/
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
