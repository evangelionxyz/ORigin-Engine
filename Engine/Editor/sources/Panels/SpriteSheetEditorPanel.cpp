// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#include "Origin/Scene/SpriteSheet.h"

#include "SpriteSheetEditorPanel.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Renderer/Renderer2D.h"
#include "Origin/Serializer/SpriteSheetSerializer.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Core/Input.h"

#include <imgui.h>
#include <cmath>

#include <glad/glad.h>

namespace origin
{
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

        Close();
    }

    void SpriteSheetEditorPanel::CreateNewSpriteSheet()
    {
        m_SpriteSheet = SpriteSheet::Create();
    }

    void SpriteSheetEditorPanel::SetSelectedSpriteSheet(const AssetHandle handle)
    {
        if (!m_CurrentFilepath.empty())
        {
            Serialize(m_CurrentFilepath);
        }

        Reset();

        m_SpriteSheet = AssetManager::GetAsset<SpriteSheet>(handle);
        m_CurrentFilepath = Project::GetActiveAssetDirectory() / Project::GetActive()->GetEditorAssetManager()->GetFilepath(handle);

        if (Deserialize() && !m_Open)
        {
            m_Camera.SetOrthoScale(static_cast<f32>(m_Texture->GetHeight()) * 1.5f);
            m_Camera.SetOrthoScaleMax(static_cast<f32>(m_Texture->GetHeight()) * 3.0f);
            m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 2.f));
            ImGui::SetWindowFocus("Sprite Sheet Editor");
            Open();
        }
    }

    void SpriteSheetEditorPanel::SetMainTexture(const AssetHandle handle) const
    {
        if (m_SpriteSheet)
        {
            m_SpriteSheet->SetMainTexture(handle);
        }
    }

    void SpriteSheetEditorPanel::AddSprite(const glm::vec2 &position, const glm::vec2 &size, const Rect &rect) const
    {
        SpriteSheetData sprite {};
        sprite.rect = rect;
        m_SpriteSheet->Sprites.push_back(sprite);
    }

    void SpriteSheetEditorPanel::RemoveSprite(const i32 index)
    {
        m_Controls.erase(m_Controls.begin() + index);
        if (!m_Controls.empty())
            m_SelectedIndex = static_cast<i32>(m_Controls.size()) - 1;
        else
            m_SelectedIndex = -1;
    }

    void SpriteSheetEditorPanel::Duplicate(i32 index)
    {
        m_Controls.insert(m_Controls.end(), m_Controls[index]);
        m_SelectedIndex = 0;
    }

    void SpriteSheetEditorPanel::Render()
    {
        if (m_Open)
        {
            ImGui::Begin("Sprite Sheet Editor", &m_Open, ImGuiWindowFlags_NoScrollbar
                | ImGuiWindowFlags_NoScrollWithMouse);

            IsViewportFocused = ImGui::IsWindowFocused();
            IsViewportHovered = ImGui::IsWindowHovered();
            
            const ImVec2 &viewportMinRegion = ImGui::GetWindowContentRegionMin();
            const ImVec2 &viewportMaxRegion = ImGui::GetWindowContentRegionMax();
            const ImVec2 &viewportOffset = ImGui::GetWindowPos();

            m_ViewportRect.min = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
            m_ViewportRect.max = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
            
            // Framebuffer Texture
            auto texture = reinterpret_cast<void*>(static_cast<uintptr_t>(m_Framebuffer->GetColorAttachmentRendererID()));
            ImGui::Image(texture, { m_ViewportRect.GetSize().x, m_ViewportRect.GetSize().y }, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::End();

            ImGui::Begin("Sprite Sheet Inspector");

            if (m_Texture)
            {
                texture = reinterpret_cast<void*>(static_cast<uintptr_t>(m_Texture->GetID()));
                const glm::vec2 atlas_size { static_cast<f32>(m_Texture->GetWidth()), static_cast<f32>(m_Texture->GetHeight()) };

                if (ImGui::Button("Save"))
                {
                    Serialize(m_CurrentFilepath);
                    OGN_CORE_TRACE("[Sprite Sheet Editor] Saved in {}", m_CurrentFilepath.generic_string());
                }

                ImGui::SameLine();
                if (ImGui::Button("Add"))
                {
                    SpriteSheetController control;
                    control.size = glm::vec2(m_Camera.GetOrthoScale() * 0.25f);
                    control.position = glm::vec2(0.0f); //glm::vec2(m_Camera.GetPosition());
                    m_MoveTranslation = control.position;
                    m_Controls.push_back(control);
                    m_SelectedIndex = static_cast<i32>(m_Controls.size()) - 1;
                }

                ImGui::Text("Atlas Size: %.2f, %.2f", atlas_size.x, atlas_size.y);

                i32 offset = 0;
                for (i32 i = 0; i < m_Controls.size(); i++)
                {
                    if (m_SelectedIndex == offset / 5)
                    {
                        auto &[position, size, rect, corner, selected_corner] = m_Controls[m_SelectedIndex];
                        rect.min = { (position.x + (atlas_size.x - size.x) / 2.0f) /  atlas_size.x, (position.y + (atlas_size.y - size.y) / 2.0f) / atlas_size.y };
                        rect.max = { (position.x + (atlas_size.x + size.x) / 2.0f) / atlas_size.x, (position.y + (atlas_size.y + size.y) / 2.0f) / atlas_size.y };
                    }
                    offset += 5;
                }

                constexpr f32 thumbnail_size = 60.0f;
                constexpr f32 padding = 10.0f;
                constexpr f32 cell_size = thumbnail_size + padding;
                const f32 panel_width = ImGui::GetWindowContentRegionMax().x;
                i32 column_count = static_cast<i32>(panel_width / cell_size);
                if (column_count < 1)
                    column_count = 1;

                ImGui::Columns(column_count, nullptr, false);

                // SUB SPRITE TEXTURES
                f32 thumbnailHeight = thumbnail_size * (static_cast<f32>(atlas_size.y) / static_cast<f32>(atlas_size.x));
                f32 diff = (f32)(thumbnail_size - thumbnailHeight);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + diff);

                offset = 0;
                for (i32 i = 0; i < m_Controls.size(); i++)
                {
                    auto &[position, size, rect, corner, selected_corner] = m_Controls[i];
                    ImGui::PushID(i);
                    ImGui::ImageButton("control", texture, { thumbnail_size, thumbnail_size }, { rect.min.x, rect.max.y }, { rect.max.x, rect.min.y });

                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        m_MoveTranslation = m_Controls[i].position;
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
                        ImGui::Text("Position: %f, %f ", position.x, position.y);
                        ImGui::Text("Size: %f, %f ", size.x, size.y);
                        ImGui::Text("Min: %f, %f ", rect.min.x, rect.min.y);
                        ImGui::Text("Max: %f, %f ", rect.max.x, rect.max.y);
                        ImGui::EndTooltip();
                    }

                    if (ImGui::BeginDragDropSource())
                    {
                        SpriteSheetData data;
                        data.rect = rect;
                        data.TextureHandle = m_SpriteSheet->GetTextureHandle();
                        ImGui::SetDragDropPayload("SPRITESHEET_ITEM", &data, sizeof(SpriteSheetData));
                        ImGui::EndDragDropSource();
                    }

                    ImGui::NextColumn();
                    ImGui::PopID();
                    offset += 5;
                }
            }
            ImGui::End();
        }
    }

    void SpriteSheetEditorPanel::OnUpdate(float delta_time)
    {

        // calculate mouse
        auto [mx, my] = ImGui::GetMousePos();
        m_ViewportMouse = { mx, my };
        m_ViewportMouse -= m_ViewportRect.min;
        m_ViewportMouse.y = m_ViewportRect.GetSize().y - m_ViewportMouse.y;
        m_ViewportMouse = glm::clamp(m_ViewportMouse, { 0.0f, 0.0f }, m_ViewportRect.GetSize() - glm::vec2(1.0f, 1.0f));

        if (!m_Open)
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
        m_Framebuffer->ClearAttachment(1, -1);

        const glm::vec2 &vp_size = m_ViewportRect.GetSize();
        if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification(); vp_size.x > 0.0f 
            && vp_size.y > 0.0f && (vp_size.x != static_cast<f32>(spec.Width) 
                || vp_size.y != static_cast<f32>(spec.Height)))
        {
            m_Camera.SetViewportSize(vp_size.x, vp_size.y);
            m_Framebuffer->Resize(static_cast<u32>(vp_size.x), static_cast<u32>(vp_size.y));
        }

        if (m_Texture)
        {
            glDisable(GL_DEPTH_TEST);

            Renderer2D::Begin(m_Camera);

            i32 texX = static_cast<i32>(m_Texture->GetWidth());
            i32 texY = static_cast<i32>(m_Texture->GetHeight());
            Renderer2D::DrawQuad(glm::scale(glm::mat4(1.0f), { texX, texY, -0.1f }), m_Texture);

            i32 offset = 0;
            for (auto & [position, size, rect, corner, selected_corner] : m_Controls)
            {
                bool selected = m_SelectedIndex == offset / 5;
                // Draw Rectangle Line
                glm::mat4 tf = glm::translate(glm::mat4(1.0f), { position.x, position.y, 0.8f })
                    * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
                glm::vec4 col = selected ? glm::vec4(1.0f, 1.0f, 0.0f, 0.1f) : glm::vec4(0.0f, 1.0f, 0.0f, 0.1f);
                Renderer2D::DrawQuad(tf, col);

                col = { 1.0f, 1.0f, 1.0f, 0.1f };
                Renderer2D::DrawQuad(tf, col);

                // Draw corner
                if (selected)
                {
                    f32 ortho_size = m_Camera.GetOrthoScale() * 0.03f;
                    ortho_size = std::clamp(ortho_size, 0.05f, 5.0f);

                    auto red = glm::vec4(0.8f, 0.1f, 0.1f, 0.8f);
                    auto green = glm::vec4(0.1f, 0.8f, 0.1f, 0.8f);
                    col = selected_corner == ControllerCorner::BOTTOM_LEFT ? green : red;
                    const f32 corner_size = 0.8f * ortho_size;

                    corner.top_left = Rect(
                        glm::vec2(position.x - size.x / 2.0f - corner_size, position.y + size.y / 2.0f - corner_size),
                        glm::vec2(position.x - size.x / 2.0f + corner_size, position.y + size.y / 2.0f + corner_size)
                    );

                    corner.top_right = Rect(
                        glm::vec2(position.x + size.x / 2.0f - corner_size, position.y + size.y / 2.0f - corner_size),
                        glm::vec2(position.x + size.x / 2.0f + corner_size, position.y + size.y / 2.0f + corner_size)
                    );

                    corner.bottom_left = Rect(
                        glm::vec2(position.x - size.x / 2.0f - corner_size, position.y - size.y / 2.0f - corner_size),
                        glm::vec2(position.x - size.x / 2.0f + corner_size, position.y - size.y / 2.0f + corner_size)
                    );

                    corner.bottom_right = Rect(
                        glm::vec2(position.x + size.x / 2.0f - corner_size, position.y - size.y / 2.0f - corner_size),
                        glm::vec2(position.x + size.x / 2.0f + corner_size, position.y - size.y / 2.0f + corner_size)
                    );
                    
                    Renderer2D::DrawQuad(corner.top_left, col);
                    Renderer2D::DrawQuad(corner.top_right, col);
                    Renderer2D::DrawQuad(corner.bottom_left, col);
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
        m_CurrentFilepath = filepath;
        m_SpriteSheet->Sprites.clear();
        for (auto &[position, size, rect, corner, selected_corner] : m_Controls)
        {
            SpriteSheetData data;
            data.rect = rect;
            m_SpriteSheet->Sprites.push_back(data);
        }

        return SpriteSheetSerializer::Serialize(filepath, m_SpriteSheet);
    }

    bool SpriteSheetEditorPanel::Deserialize()
    {
        const bool ret = SpriteSheetSerializer::Deserialize(m_CurrentFilepath, m_SpriteSheet);

        if (ret)
        {
            m_Texture = AssetManager::GetAsset<Texture2D>(m_SpriteSheet->GetTextureHandle());
            const glm::vec2 atlas_size { m_Texture->GetWidth(), m_Texture->GetHeight() };

            for (const auto &[rect, TextureHandle] : m_SpriteSheet->Sprites)
            {
                SpriteSheetController control;
                control.rect = rect;
                control.size.x = rect.max.x * atlas_size.x - rect.min.x * atlas_size.x;
                control.size.y = rect.max.y * atlas_size.y - rect.min.y * atlas_size.y;
                control.position.x = rect.min.x * atlas_size.x - (atlas_size.x - control.size.x) / 2.0f;
                control.position.y = rect.min.y * atlas_size.y - (atlas_size.y - control.size.y) / 2.0f;
                m_Controls.push_back(control);
            }
        }

        return ret;
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

        if (e.Is(Mouse::ButtonLeft) && IsViewportHovered)
        {
        	if (m_SelectedIndex >= 0 && !m_Controls.empty())
        	{
                if (m_Controls[m_SelectedIndex].corner.top_left.Contains(m_ViewportMouse))
                {
                    OGN_CORE_INFO("top left clicked");
                }
                else if (m_Controls[m_SelectedIndex].corner.top_right.Contains(m_ViewportMouse))
                {
                    OGN_CORE_INFO("top right clicked");
                }
                else if (m_Controls[m_SelectedIndex].corner.bottom_left.Contains(m_ViewportMouse))
                {
                    OGN_CORE_INFO("bottom left clicked");
                }
                else if (m_Controls[m_SelectedIndex].corner.bottom_left.Contains(m_ViewportMouse))
                {
                    OGN_CORE_INFO("bottom right clicked");
                }
        	}
            else if (m_SelectedIndex < 0 && !m_Controls.empty())
            {

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
        m_Camera.OnMouseScroll(e.GetYOffset());
        return false;
    }

    void SpriteSheetEditorPanel::OnMouse(f32 ts)
    {
        OGN_PROFILER_INPUT();

        const glm::vec2 delta = Input::GetMouseClickDragDelta();
        m_Camera.OnMouseMove(delta);

        if (Input::IsMouseButtonPressed(Mouse::ButtonLeft) && !m_Controls.empty() && IsViewportHovered && m_SelectedIndex >= 0)
        {
            auto &[position, size, rect, corner, selected_corner] = m_Controls[m_SelectedIndex];

            const f32 viewport_height = m_Camera.GetViewportSize().y;
            const f32 orthographic_scale = m_Camera.GetOrthoScale() / viewport_height;

            m_MoveTranslation.x -= delta.x * orthographic_scale;
            m_MoveTranslation.y += delta.y * orthographic_scale;

            if (Input::IsKeyPressed(Key::LeftShift))
            {
                f32 snap_size = 0.5f;
                if (Input::IsKeyPressed(Key::LeftControl))
                {
                    snap_size = 0.1f;
                }

                switch (selected_corner)
                {
                case NONE:
                    position.x = std::round(m_MoveTranslation.x / snap_size) * snap_size;
                    position.y = std::round(m_MoveTranslation.y / snap_size) * snap_size;
                    break;
                default: break;
                }
            }
            else
            {
                switch (selected_corner)
                {
                case NONE:
                {
                    if (Input::IsKeyPressed(Key::X))
                    {
                        position.x -= delta.x * orthographic_scale;
                    }
                    else if (Input::IsKeyPressed(Key::Y))
                    {
                        position.y += delta.y * orthographic_scale;
                    }
                    else
                    {
                        position.x -= delta.x * orthographic_scale;
                        position.y += delta.y * orthographic_scale;
                    }
                    break;
                }
                case TOP_RIGHT:
                {
                    position.x += delta.x * orthographic_scale / 2.0f;
                    size.x += delta.x * orthographic_scale;
                    position.y -= delta.y * orthographic_scale / 2.0f;
                    size.y -= delta.y * orthographic_scale;
                    break;
                }
                case BOTTOM_RIGHT:
                {
                    position.x += delta.x * orthographic_scale / 2.0f;
                    size.x += delta.x * orthographic_scale;
                    position.y -= delta.y * orthographic_scale / 2.0f;
                    size.y += delta.y * orthographic_scale;
                    break;
                }
                case TOP_LEFT:
                {
                    position.x += delta.x * orthographic_scale / 2.0f;
                    size.x -= delta.x * orthographic_scale;
                    position.y -= delta.y * orthographic_scale / 2.0f;
                    size.y -= delta.y * orthographic_scale;
                    break;
                }
                case BOTTOM_LEFT:
                {
                    position.x += delta.x * orthographic_scale / 2.0f;
                    size.x -= delta.x * orthographic_scale;
                    position.y -= delta.y * orthographic_scale / 2.0f;
                    size.y += delta.y * orthographic_scale;
                    break;
                }
                }
            }
        }
    }

    SpriteSheetEditorPanel *SpriteSheetEditorPanel::GetInstance()
    {
        return s_Instance;
    }

    void SpriteSheetEditorPanel::Reset()
    {
        if (m_SpriteSheet)
        {
            m_SpriteSheet->Sprites.clear();
            m_SpriteSheet.reset();
        }

        m_Controls.clear();
    }
}
