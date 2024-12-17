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
            m_SpriteSheet->SetMainTexture(handle);
    }

    void SpriteSheetEditorPanel::AddSprite(const glm::vec2 position, const glm::vec2 size,
        const glm::vec2 min, const glm::vec2 max) const
    {
        SpriteSheetData sprite {};
        sprite.Min = min;
        sprite.Max = max;
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
                    control.Size = glm::vec2(m_Camera.GetOrthoScale() * 0.25f);
                    control.Position = glm::vec2(m_Camera.GetPosition());
                    m_MoveTranslation = control.Position;
                    m_Controls.push_back(control);
                    m_SelectedIndex = static_cast<i32>(m_Controls.size()) - 1;
                }

                ImGui::Text("Atlas Size: %.2f, %.2f", atlas_size.x, atlas_size.y);

                i32 offset = 0;
                for (i32 i = 0; i < m_Controls.size(); i++)
                {
                    if (m_SelectedIndex == offset / 5)
                    {
                        auto & [Position, Size, Min, Max, SelectedCorner] = m_Controls[m_SelectedIndex];
                        Min = { (Position.x + (atlas_size.x - Size.x) / 2.0f) /  atlas_size.x, (Position.y + (atlas_size.y - Size.y) / 2.0f) / atlas_size.y };
                        Max = { (Position.x + (atlas_size.x + Size.x) / 2.0f) / atlas_size.x, (Position.y + (atlas_size.y + Size.y) / 2.0f) / atlas_size.y };
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
                    auto & [Position, Size, Min, Max, SelectedCorner] = m_Controls[i];
                    ImGui::PushID(i);
                    ImGui::ImageButton("control", texture, { thumbnail_size, thumbnail_size }, { Min.x, Max.y }, { Max.x, Min.y });

                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    {
                        m_MoveTranslation = m_Controls[i].Position;
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
                        ImGui::Text("Position: %f, %f ", Position.x, Position.y);
                        ImGui::Text("Size: %f, %f ", Size.x, Size.y);
                        ImGui::Text("Min: %f, %f ", Min.x, Min.y);
                        ImGui::Text("Max: %f, %f ", Max.x, Max.y);
                        ImGui::EndTooltip();
                    }

                    if (ImGui::BeginDragDropSource())
                    {
                        SpriteSheetData data;
                        data.Min = Min;
                        data.Max = Max;
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
        if (!m_Open)
        {
            return;
        }

        if (IsViewportFocused && IsViewportHovered)
            m_Camera.OnUpdate(delta_time);

        m_Camera.UpdateView();
        m_Camera.UpdateProjection();

        OnMouse(delta_time);

        RenderCommand::ClearColor(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f));
        m_Framebuffer->Bind();
        RenderCommand::Clear();
        m_Framebuffer->ClearAttachment(1, -1);

        const glm::vec2 &vp_size = m_ViewportRect.GetSize();
        if (const FramebufferSpecification spec = m_Framebuffer->GetSpecification(); vp_size.x > 0.0f && vp_size.y > 0.0f && (vp_size.x != static_cast<f32>(spec.Width) || vp_size.y != static_cast<f32>(spec.Height)))
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
            for (auto & [Position, Size, Min, Max, SelectedCorner] : m_Controls)
            {
                bool selected = m_SelectedIndex == offset / 5;

                // Draw Rectangle Line
                glm::mat4 tf = glm::translate(glm::mat4(1.0f), { Position.x, Position.y, 0.8f })
                    * glm::scale(glm::mat4(1.0f), { Size.x, Size.y, 1.0f });
                glm::vec4 col = selected ? glm::vec4(1.0f, 1.0f, 0.0f, 0.1f) : glm::vec4(0.0f, 1.0f, 0.0f, 0.1f);
                Renderer2D::DrawQuad(tf, col);
                col = { 1.0f, 1.0f, 1.0f, 0.1f };
                Renderer2D::DrawQuad(tf, col);

                // Draw corner
                if (selected)
                {
                    f32 size = m_Camera.GetOrthoScale() * 0.03f;
                    size = std::min(size, 5.0f);
                    size = std::max(size, 0.05f);

                    // bottom left corner
                    auto red = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
                    auto green = glm::vec4(0.1f, 0.8f, 0.1f, 1.0f);
                    col = SelectedCorner == ControllerCorner::BOTTOM_LEFT ? green : red;

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), { Position.x - Size.x / 2.0f, Position.y - Size.y / 2.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), glm::vec3(size));
                    Renderer2D::DrawQuad(transform, col);

                    // top left corner
                    col = SelectedCorner == ControllerCorner::TOP_LEFT ? green : red;
                    transform = glm::translate(glm::mat4(1.0f), { Position.x - Size.x / 2.0f, Position.y + Size.y / 2.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), glm::vec3(size));
                    Renderer2D::DrawQuad(transform, col);

                    // bottom right corner
                    col = SelectedCorner == ControllerCorner::BOTTOM_RIGHT ? green : red;
                    transform = glm::translate(glm::mat4(1.0f), { Position.x + Size.x / 2.0f, Position.y - Size.y / 2.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), glm::vec3(size));
                    Renderer2D::DrawQuad(transform, col);

                    // top right corner
                    col = SelectedCorner == ControllerCorner::TOP_RIGHT ? green : red;
                    transform = glm::translate(glm::mat4(1.0f), { Position.x + Size.x / 2.0f, Position.y + Size.y / 2.0f, 1.0f }) * glm::scale(glm::mat4(1.0f), glm::vec3(size));
                    Renderer2D::DrawQuad(transform, col);

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
        for (auto & [Position, Size, Min, Max, SelectedCorner] : m_Controls)
        {
            SpriteSheetData data;
            data.Min = Min;
            data.Max = Max;
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

            for (const auto &[Min, Max, TextureHandle] : m_SpriteSheet->Sprites)
            {
                SpriteSheetController control;
                control.Min = Min;
                control.Max = Max;
                control.Size.x = Max.x * atlas_size.x - Min.x * atlas_size.x;
                control.Size.y = Max.y * atlas_size.y - Min.y * atlas_size.y;
                control.Position.x = Min.x * atlas_size.x - (atlas_size.x - control.Size.x) / 2.0f;
                control.Position.y = Min.y * atlas_size.y - (atlas_size.y - control.Size.y) / 2.0f;
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

        // TODO: Fix Me!
        //if (e.Is(Mouse::ButtonLeft) && IsViewportHovered)
        //{
        //	if (m_HoveredIndex != (m_SelectedIndex == 0 ? -1 : m_SelectedIndex) && m_HoveredIndex >= 0)
        //	{
        //		i32 h = m_HoveredIndex / 5;
        //		m_SelectedIndex = h;
        //		if (m_HoveredIndex == h * 5 + 1)
        //			m_Controls[m_SelectedIndex].SelectedCorner = BOTTOM_LEFT;
        //		else if (m_HoveredIndex == h * 5 + 2)
        //			m_Controls[m_SelectedIndex].SelectedCorner = TOP_LEFT;
        //		else if (m_HoveredIndex == h * 5 + 3)
        //			m_Controls[m_SelectedIndex].SelectedCorner = BOTTOM_RIGHT;
        //		else if (m_HoveredIndex == h * 5 + 4)
        //			m_Controls[m_SelectedIndex].SelectedCorner = TOP_RIGHT;
        //		else
        //		{
        //			m_Controls[m_SelectedIndex].SelectedCorner = NONE;
        //			m_MoveTranslation = m_Controls[m_SelectedIndex].Position;
        //		}
        //	}
        //	else if (m_HoveredIndex < 0 && m_SelectedIndex >= 0)
        //	{
        //		m_Controls[m_SelectedIndex].SelectedCorner = NONE;
        //		//m_SelectedIndex = -1;
        //	}
        //}

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

        return false;
    }

    void SpriteSheetEditorPanel::OnMouse(f32 ts)
    {
        OGN_PROFILER_INPUT();

        if (m_Controls.empty())
        {
            return;
        }
        [[maybe_unused]] const glm::vec2 mouse { Input::GetMouseX(), Input::GetMouseY() };
        const glm::vec2 delta = Input::GetMouseClickDragDelta();

        if (Input::IsMouseButtonPressed(Mouse::ButtonLeft) && IsViewportHovered && m_SelectedIndex >= 0)
        {
            auto &[Position, Size, Min, Max, SelectedCorner] = m_Controls[m_SelectedIndex];
            const f32 viewport_height = m_Camera.GetViewportSize().y;
            const f32 orthographic_scale = m_Camera.GetOrthoScale() / viewport_height;

            m_MoveTranslation.x += delta.x * orthographic_scale;
            m_MoveTranslation.y -= delta.y * orthographic_scale;

            if (Input::IsKeyPressed(Key::LeftShift))
            {
                f32 snap_size = 0.5f;
                if (Input::IsKeyPressed(Key::LeftControl))
                {
                    snap_size = 0.1f;
                }

                switch (SelectedCorner)
                {
                case NONE:
                    Position.x = std::round(m_MoveTranslation.x / snap_size) * snap_size;
                    Position.y = std::round(m_MoveTranslation.y / snap_size) * snap_size;
                    break;
                default: break;
                }
            }
            else
            {
                switch (SelectedCorner)
                {
                case NONE:
                    if (Input::IsKeyPressed(Key::X))
                    {
                        Position.x += delta.x * orthographic_scale;
                    }
                    else if (Input::IsKeyPressed(Key::Y))
                    {
                        Position.y -= delta.y * orthographic_scale;
                    }
                    else
                    {
                        Position.x += delta.x * orthographic_scale;
                        Position.y -= delta.y * orthographic_scale;
                    }
                    break;
                case TOP_RIGHT:
                    Position.x += delta.x * orthographic_scale / 2.0f;
                    Size.x += delta.x * orthographic_scale;
                    Position.y -= delta.y * orthographic_scale / 2.0f;
                    Size.y -= delta.y * orthographic_scale;
                    break;
                case BOTTOM_RIGHT:
                    Position.x += delta.x * orthographic_scale / 2.0f;
                    Size.x += delta.x * orthographic_scale;
                    Position.y -= delta.y * orthographic_scale / 2.0f;
                    Size.y += delta.y * orthographic_scale;
                    break;
                case TOP_LEFT:
                    Position.x += delta.x * orthographic_scale / 2.0f;
                    Size.x -= delta.x * orthographic_scale;
                    Position.y -= delta.y * orthographic_scale / 2.0f;
                    Size.y -= delta.y * orthographic_scale;
                    break;
                case BOTTOM_LEFT:
                    Position.x += delta.x * orthographic_scale / 2.0f;
                    Size.x -= delta.x * orthographic_scale;
                    Position.y -= delta.y * orthographic_scale / 2.0f;
                    Size.y += delta.y * orthographic_scale;
                    break;
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
