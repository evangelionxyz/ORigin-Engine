// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "Origin/Scene/SpriteSheet.h"
#include "Origin/GUI/UI.h"
#include "SpriteSheetEditorPanel.hpp"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/Renderer2D.h"
#include "Origin/Serializer/SpriteSheetSerializer.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Core/Input.h"

#include <imgui.h>
#include <algorithm>
#include <cmath>

#include <glad/glad.h>

#include "Origin/Profiler/Profiler.h"

namespace origin {
static SpriteSheetEditorPanel *s_Instance = nullptr;

SpriteSheetEditorPanel::SpriteSheetEditorPanel()
{
    s_Instance = this;

    m_Camera.InitOrthographic(10.0f, 0.0f, 2.0f);
    m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));

    FramebufferSpecification spec;
    spec.Attachments =
    {
        FramebufferTextureFormat::RGBA8,
    };

    spec.Width = 1280;
    spec.Height = 720;

    m_Framebuffer = Framebuffer::Create(spec);

    m_SpriteSheet = SpriteSheet::Create();

    Close();
}

Ref<SpriteSheet> SpriteSheetEditorPanel::CreateSpriteSheet(const AssetHandle texture_handle)
{
    m_SpriteSheet = SpriteSheet::Create();
    m_SpriteSheet->SetTexture(texture_handle);

    return m_SpriteSheet;
}

void SpriteSheetEditorPanel::OpenSpriteSheet(const AssetHandle sprite_sheet_handle)
{
    if (!m_CurrentFilepath.empty())
    {
        Serialize(m_CurrentFilepath);
    }

    m_SpriteSheet = AssetManager::GetAsset<SpriteSheet>(sprite_sheet_handle);

    m_CurrentFilepath = Project::GetActiveAssetDirectory() / Project::GetActive()->GetEditorAssetManager()->GetFilepath(sprite_sheet_handle);

    LoadSpritesToController();

    m_Texture = AssetManager::GetAsset<Texture2D>(m_SpriteSheet->GetTextureHandle());

    if (m_Texture)
    {
        const f32 max_scale = std::max(m_Texture->GetHeight(), m_Texture->GetWidth());
        m_Camera.SetOrthoScale(max_scale * 1.5f);
        m_Camera.SetOrthoScaleMax(max_scale * 3.0f);
        m_Camera.SetPosition({ 0.0f, 0.0f, 2.f });

    }

    ImGui::SetWindowFocus("Sprite Sheet Editor");
    Open();
}


void SpriteSheetEditorPanel::RemoveSprite(const i32 index)
{
    m_Controls.erase(m_Controls.begin() + index);
    if (!m_Controls.empty())
    {
        m_SelectedIndex = static_cast<i32>(m_Controls.size()) - 1;
    }
    else
    {
        m_SelectedIndex = -1;
    }
}

void SpriteSheetEditorPanel::Duplicate(i32 index)
{
    m_Controls.insert(m_Controls.end(), m_Controls[index]);
    m_SelectedIndex = m_Controls.size() - 1;
}

void SpriteSheetEditorPanel::Render()
{
    if (m_is_open)
    {
        ImGui::Begin("Sprite Sheet Editor", &m_is_open);

        const ImVec2 &window_region = ImGui::GetContentRegionAvail();
        ImGui::BeginChild("sprite_sheet_inspector", {300.0f, 0.0f}, ImGuiChildFlags_ResizeX);
        const f32 inspector_width = ImGui::GetWindowWidth();
            
        if (m_Texture)
        {
            const ImVec2 button_size = ImVec2(80.0f, 25.0f);
            
            auto texture = reinterpret_cast<void*>(static_cast<uintptr_t>(m_Texture->GetID()));
            const glm::vec2 atlas_size { static_cast<f32>(m_Texture->GetWidth()), static_cast<f32>(m_Texture->GetHeight()) };

            UI::DrawFloatControl("Grid X", &m_SpriteSheet->m_GridSize.x);
            UI::DrawFloatControl("Grid Y", &m_SpriteSheet->m_GridSize.y);

            m_SpriteSheet->m_GridSize = glm::max({ 1.0f, 1.0f }, m_SpriteSheet->m_GridSize);
            
            if (ImGui::Button("Apply", {inspector_width, button_size.y}))
            {
                m_Controls.clear();

                i32 x_count = static_cast<i32>(atlas_size.x) / static_cast<u32>(m_SpriteSheet->m_GridSize.x);
                i32 y_count = static_cast<i32>(atlas_size.y) / static_cast<u32>(m_SpriteSheet->m_GridSize.y);
                
                if (x_count == 0)
                {
                    x_count = 1;
                    m_SpriteSheet->m_GridSize.x = atlas_size.x;
                }
                
                if (y_count == 0)
                {
                    y_count = 1;
                    m_SpriteSheet->m_GridSize.y = atlas_size.y;
                }
                
                for (i32 i = 0; i < x_count; ++i)
                {
                    for (i32 j = 0; j < y_count; ++j)
                    {
                        SpriteSheetController sprite_controller;
                        sprite_controller.data.rect.min.x = i * m_SpriteSheet->m_GridSize.x - atlas_size.x / 2.0f;
                        sprite_controller.data.rect.min.y = j * m_SpriteSheet->m_GridSize.y - atlas_size.y / 2.0f;
                        sprite_controller.data.rect.max.x = static_cast<f32>(i + 1) * m_SpriteSheet->m_GridSize.x - atlas_size.x / 2.0f;
                        sprite_controller.data.rect.max.y = static_cast<f32>(j) * m_SpriteSheet->m_GridSize.y + atlas_size.y / 2.0f;

                        sprite_controller.data.texture_handle = m_Texture->GetID();
                        sprite_controller.data.atlas_size = atlas_size;

                        const glm::vec2 &pos = sprite_controller.data.rect.GetCenter();
                        const glm::vec2 &size = sprite_controller.data.rect.GetSize();

                        sprite_controller.data.uv0 = { (pos.x + (atlas_size.x - size.x) / 2.0f) / atlas_size.x, (pos.y + (atlas_size.y - size.y) / 2.0f) / atlas_size.y };
                        sprite_controller.data.uv1 = { (pos.x + (atlas_size.x + size.x) / 2.0f) / atlas_size.x, (pos.y + (atlas_size.y + size.y) / 2.0f) / atlas_size.y };
                    
                        m_Controls.push_back(sprite_controller);
                    }
                }
            }
            ImGui::Separator();
            
            if (ImGui::Button("Save", button_size))
            {
                Serialize(m_CurrentFilepath);
                OGN_CORE_TRACE("[Sprite Sheet Editor] Saved in {}", m_CurrentFilepath.generic_string());
            }

            ImGui::SameLine();
            if (ImGui::Button("Create", button_size))
            {
                SpriteSheetController control;

                f32 rect_size = std::max(atlas_size.x, atlas_size.y);
                control.data.rect = Rect({0.0f, 0.0f}, {rect_size, rect_size});
                
                control.data.texture_handle = m_Texture->GetID();
                control.data.atlas_size = atlas_size;
                
                const glm::vec2 &pos = control.data.rect.GetCenter();
                const glm::vec2 &size = control.data.rect.GetSize();
                control.data.uv0 = { (pos.x + (atlas_size.x - size.x) / 2.0f) / atlas_size.x, (pos.y + (atlas_size.y - size.y) / 2.0f) / atlas_size.y };
                control.data.uv1 = { (pos.x + (atlas_size.x + size.x) / 2.0f) / atlas_size.x, (pos.y + (atlas_size.y + size.y) / 2.0f) / atlas_size.y };
                
                m_Controls.push_back(control);
                m_SelectedIndex = static_cast<i32>(m_Controls.size()) - 1;
            }

            ImGui::Text("Atlas Size: %.2f, %.2f", atlas_size.x, atlas_size.y);

            constexpr f32 thumbnail_size = 60.0f;
            constexpr f32 padding = 12.0f;
            constexpr f32 cell_size = thumbnail_size + padding;
            i32 column_count = static_cast<i32>(inspector_width / cell_size);
            column_count = std::max(column_count, 1);

            ImGui::Columns(column_count, nullptr, false);

            // SUB SPRITE TEXTURES
            const f32 thumbnail_height = thumbnail_size * (static_cast<f32>(atlas_size.y) / static_cast<f32>(atlas_size.x));
            const f32 diff = thumbnail_size - thumbnail_height;
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + diff);

            i32 offset = 0;
            for (size_t i = 0; i < m_Controls.size(); i++)
            {
                auto &[data, corner, selected_corner] = m_Controls[i];

                ImGui::PushID(i);
                ImGui::ImageButton("control", texture, { thumbnail_size, thumbnail_size }, { data.uv0.x, data.uv1.y }, { data.uv1.x, data.uv0.y });

                if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    m_SelectedIndex = i;
                }

                if (ImGui::BeginPopupContextItem("sprite_context", 1))
                {
                    if (ImGui::MenuItem("Delete"))
                        RemoveSprite(i);

                    if (ImGui::MenuItem("Duplicate"))
                        Duplicate(i);

                    ImGui::EndPopup();
                }

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Text("Position: %f, %f ", data.rect.GetCenter().x, data.rect.GetCenter().x);
                    ImGui::Text("Size: %f, %f ", data.rect.GetSize().x, data.rect.GetSize().x);
                    ImGui::Text("Rectangle: %f, %f, %f, %f ", data.rect.min.x, data.rect.min.y, data.rect.max.x, data.rect.max.y);
                    ImGui::EndTooltip();
                }

                if (ImGui::BeginDragDropSource())
                {;
                    ImGui::SetDragDropPayload("SPRITESHEET_ITEM", &data, sizeof(SpriteSheetData));
                    ImGui::EndDragDropSource();
                }

                ImGui::NextColumn();
                ImGui::PopID();
                offset += 5;
            }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("sprite_sheet_viewport", {window_region.x - inspector_width, 0.0f}, 0);
        {
            IsViewportFocused = ImGui::IsWindowFocused();
            IsViewportHovered = ImGui::IsWindowHovered();
            const ImVec2 &vp_min_region = ImGui::GetWindowContentRegionMin();
            const ImVec2 &vp_max_region = ImGui::GetWindowContentRegionMax();
            const ImVec2 &vp_offset = ImGui::GetWindowPos();
            m_viewport_rect.min = { vp_min_region.x + vp_offset.x, vp_min_region.y + vp_offset.y };
            m_viewport_rect.max = { vp_max_region.x + vp_offset.x, vp_max_region.y + vp_offset.y };
            
            // Framebuffer Texture
            auto texture = reinterpret_cast<void*>(static_cast<uintptr_t>(m_Framebuffer->GetColorAttachmentRendererID()));
            ImGui::Image(texture, { m_viewport_rect.GetSize().x, m_viewport_rect.GetSize().y }, ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::EndChild();

        ImGui::End();
    }
}

void SpriteSheetEditorPanel::OnUpdate(float delta_time)
{
    // calculate mouse
    auto [mx, my] = ImGui::GetMousePos();
    m_viewport_mouse = { mx, my };
    m_viewport_mouse -= m_viewport_rect.min;
    m_viewport_mouse.y = m_viewport_rect.GetSize().y - m_viewport_mouse.y;
    m_viewport_mouse = glm::clamp(m_viewport_mouse, { 0.0f, 0.0f }, m_viewport_rect.GetSize() - glm::vec2(1.0f, 1.0f));

    if (!m_is_open)
    {
        return;
    }

    if (IsViewportFocused && IsViewportHovered)
    {
        m_Camera.OnUpdate(delta_time);
        OnMouse(delta_time);
    }

    m_Camera.UpdateView();
    m_Camera.UpdateProjection();

    OnMouse(delta_time);

    RenderCommand::ClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
    m_Framebuffer->Bind();
    RenderCommand::Clear();

    const glm::vec2 &vp_size = m_viewport_rect.GetSize();
    if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification(); vp_size.x > 0.0f 
        && vp_size.y > 0.0f && (vp_size.x != static_cast<f32>(spec.Width) || vp_size.y != static_cast<f32>(spec.Height)))
    {
        m_Camera.SetViewportSize(vp_size.x, vp_size.y);
        m_Framebuffer->Resize(static_cast<u32>(vp_size.x), static_cast<u32>(vp_size.y));
    }

    if (m_Texture)
    {
        glDisable(GL_DEPTH_TEST);

        Renderer2D::Begin();

        const i32 tex_x = static_cast<i32>(m_Texture->GetWidth());
        const i32 tex_y = static_cast<i32>(m_Texture->GetHeight());

        // outline
        Renderer2D::DrawRect({0.0f, 0.0f, 1.0f}, {tex_x, tex_y}, glm::vec4(1.0f));
            
        // texture
        Renderer2D::DrawQuad(glm::scale(glm::mat4(1.0f), { tex_x, tex_y, -0.1f }), m_Texture);

        i32 offset = 0;
        for (auto & [data, corner, selected_corner] : m_Controls)
        {
            bool selected = m_SelectedIndex == offset / 5;
                
            glm::vec4 col = selected ? glm::vec4(1.0f, 1.0f, 0.0f, 0.1f) : glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            glm::vec4 outline_col = selected ? glm::vec4(0.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(1.0f, 0.5f, 0.5f, 1.0f);
            
            Renderer2D::DrawRect({data.rect.GetCenter(), 0.0f}, data.rect.GetSize(), outline_col);

            Renderer2D::DrawQuad(data.rect, col);

            // Draw corner
            if (selected)
            {
                f32 ortho_size = m_Camera.GetOrthoScale() * 0.03f;
                ortho_size = std::clamp(ortho_size, 0.05f, 5.0f);

                auto red = glm::vec4(0.8f, 0.1f, 0.1f, 0.8f);
                auto green = glm::vec4(0.1f, 0.8f, 0.1f, 0.8f);
                const f32 corner_size = 0.8f * ortho_size;

                corner.top_left = Rect(
                    glm::vec2(data.rect.min.x - corner_size, data.rect.max.y - corner_size),
                    glm::vec2(data.rect.min.x + corner_size, data.rect.max.y + corner_size)
                );

                corner.top_right = Rect(
                    glm::vec2(data.rect.max.x - corner_size, data.rect.max.y - corner_size),
                    glm::vec2(data.rect.max.x + corner_size, data.rect.max.y + corner_size)
                );

                corner.bottom_left = Rect(
                    glm::vec2(data.rect.min.x - corner_size, data.rect.min.y - corner_size),
                    glm::vec2(data.rect.min.x + corner_size, data.rect.min.y + corner_size)
                );

                corner.bottom_right = Rect(
                    glm::vec2(data.rect.max.x - corner_size, data.rect.min.y - corner_size),
                    glm::vec2(data.rect.max.x + corner_size, data.rect.min.y + corner_size)
                );
                    
                col = selected_corner == ControllerCorner::TOP_LEFT ? green : red;
                Renderer2D::DrawQuad(corner.top_left, col);
                col = selected_corner == ControllerCorner::TOP_RIGHT ? green : red;
                Renderer2D::DrawQuad(corner.top_right, col);
                col = selected_corner == ControllerCorner::BOTTOM_LEFT ? green : red;
                Renderer2D::DrawQuad(corner.bottom_left, col);
                col = selected_corner == ControllerCorner::BOTTOM_RIGHT ? green : red;
                Renderer2D::DrawQuad(corner.bottom_right, col);
            }
            offset += 5;
        }

        Renderer2D::End();
        glEnable(GL_DEPTH_TEST);
    }

    m_Framebuffer->Unbind();
}

bool SpriteSheetEditorPanel::Serialize(const std::filesystem::path &filepath)
{
    if (m_SpriteSheet)
    {
        m_CurrentFilepath = filepath;

        m_SpriteSheet->Sprites.clear();
        for (auto &[data, corner, selected_corner] : m_Controls)
        {
            m_SpriteSheet->Sprites.push_back(data);
        }

        return SpriteSheetSerializer::Serialize(filepath, m_SpriteSheet);
    }

    return false;
}

bool SpriteSheetEditorPanel::LoadSpritesToController()
{
    m_Controls.clear();

    for (const auto &sprite : m_SpriteSheet->Sprites)
    {
        SpriteSheetController control;
        control.data = sprite;
        m_Controls.push_back(control);
    }

    return !m_Controls.empty();
}

void SpriteSheetEditorPanel::OnEvent(Event &e)
{
    m_Camera.OnEvent(e);
    EventDispatcher dispatcher(e);

    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(SpriteSheetEditorPanel::OnMouseButtonPressed));
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(SpriteSheetEditorPanel::OnKeyPressed));
    dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(SpriteSheetEditorPanel::OnMouseScroll));
}

bool SpriteSheetEditorPanel::OnMouseButtonPressed(MouseButtonPressedEvent &e)
{
    if (IsViewportHovered && !IsViewportFocused)
    {
        if (e.Is(Mouse::ButtonMiddle) || e.Is(Mouse::ButtonRight) || e.Is(Mouse::ButtonLeft))
        {
            ImGui::SetWindowFocus("Sprite Sheet Editor");
        }
    }

    auto ray_direction = glm::vec3(0.0f);
    auto ray_origin = glm::vec3(0.0f);

    ray_direction = Math::GetRayFromScreenCoords(m_viewport_mouse, m_viewport_rect.GetSize(),
        m_Camera.GetProjectionMatrix(),
        m_Camera.GetViewMatrix(),
        m_Camera.IsPerspective(),
        ray_origin);

    if (e.Is(Mouse::ButtonLeft) && IsViewportHovered)
    {
        for (size_t i = 0; i < m_Controls.size(); ++i)
        {
            if (m_Controls[i].data.rect.Contains(ray_origin))
            {
                m_SelectedIndex = i;
                m_Controls[i].selected_corner = NONE;
                break;
            }
        }

        if (m_SelectedIndex >= 0 && !m_Controls.empty())
        {
            if (m_Controls[m_SelectedIndex].corner.top_left.Contains(ray_origin))
            {
                m_Controls[m_SelectedIndex].selected_corner = TOP_LEFT;
            }
            else if (m_Controls[m_SelectedIndex].corner.top_right.Contains(ray_origin))
            {
                m_Controls[m_SelectedIndex].selected_corner = TOP_RIGHT;
            }
            else if (m_Controls[m_SelectedIndex].corner.bottom_left.Contains(ray_origin))
            {
                m_Controls[m_SelectedIndex].selected_corner = BOTTOM_LEFT;
            }
            else if (m_Controls[m_SelectedIndex].corner.bottom_right.Contains(ray_origin))
            {
                m_Controls[m_SelectedIndex].selected_corner = BOTTOM_RIGHT;
            }
            else
            {
                m_Controls[m_SelectedIndex].selected_corner = NONE;
            }
        }
    }

    return false;
}

bool SpriteSheetEditorPanel::OnKeyPressed(const KeyPressedEvent &e)
{
    OGN_PROFILER_INPUT();

    if (!IsViewportFocused)
        return false;

    if (Input::IsKeyPressed(Key::LeftControl))
    {
        if (e.GetKeyCode() == Key::D && m_SelectedIndex >= 0 && !m_Controls.empty())
        {
            Duplicate(m_SelectedIndex);
        }

        if (e.GetKeyCode() == Key::S)
        {
            Serialize(m_CurrentFilepath);
            OGN_CORE_TRACE("[SpriteSheetEditor] Saved in {}", m_CurrentFilepath.generic_string());
        }
    }

    if (e.GetKeyCode() == Key::Delete && m_SelectedIndex >= 0 && !m_Controls.empty())
    {
        RemoveSprite(m_SelectedIndex);
    }

    return false;
}

bool SpriteSheetEditorPanel::OnMouseScroll(const MouseScrolledEvent &e)
{
    if (IsViewportHovered)
        m_Camera.OnMouseScroll(e.GetYOffset());
    return false;
}

void SpriteSheetEditorPanel::OnMouse(f32 ts)
{
    OGN_PROFILER_INPUT();

    const glm::vec2 delta = Input::GetMouseClickDragDelta();

    if (IsViewportHovered)
    {
        m_Camera.OnMouseMove(delta);
    }

    if (Input::IsMouseButtonPressed(Mouse::ButtonLeft) && !m_Controls.empty() && IsViewportHovered && m_SelectedIndex >= 0)
    {
        auto &[data, corner, selected_corner] = m_Controls[m_SelectedIndex];

        const f32 viewport_height = m_Camera.GetViewportSize().y;
        const f32 orthographic_scale = m_Camera.GetOrthoScale() / viewport_height;

        switch (selected_corner)
        {
        case NONE:
        {
            if (Input::IsKeyPressed(Key::X))
            {
                data.rect.min.x -= delta.x * orthographic_scale / 2.0f;
                data.rect.max.x -= delta.x * orthographic_scale / 2.0f;
            }
            else if (Input::IsKeyPressed(Key::Y))
            {
                data.rect.min.y += delta.y * orthographic_scale / 2.0f;
                data.rect.max.y += delta.y * orthographic_scale / 2.0f;
            }
            else
            {
                data.rect.min.x -= delta.x * orthographic_scale / 2.0f;
                data.rect.max.x -= delta.x * orthographic_scale / 2.0f;

                data.rect.min.y += delta.y * orthographic_scale / 2.0f;
                data.rect.max.y += delta.y * orthographic_scale / 2.0f;
            }
            break;
        }
        case TOP_LEFT:
        {
            data.rect.min.x -= delta.x * orthographic_scale / 2.0f;
            data.rect.max.y += delta.y * orthographic_scale / 2.0f;
            break;
        }
        case TOP_RIGHT:
        {
            data.rect.max.x -= delta.x * orthographic_scale / 2.0f;
            data.rect.max.y += delta.y * orthographic_scale / 2.0f;
            break;
        }
        case BOTTOM_LEFT:
        {
            data.rect.min.x -= delta.x * orthographic_scale / 2.0f;
            data.rect.min.y += delta.y * orthographic_scale / 2.0f;
            break;
        }
        case BOTTOM_RIGHT:
        {
            data.rect.max.x -= delta.x * orthographic_scale / 2.0f;
            data.rect.min.y += delta.y * orthographic_scale / 2.0f;
            break;
        }
        }
    }
}

SpriteSheetEditorPanel *SpriteSheetEditorPanel::GetInstance()
{
    return s_Instance;
}

}
