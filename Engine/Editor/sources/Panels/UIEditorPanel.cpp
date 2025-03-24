#include "UIEditorPanel.hpp"
#include "../EditorLayer.hpp"
#include "Origin/GUI/UI.h"
#include "Origin/Renderer/2D/Renderer2D.hpp"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Scene/Components/Components.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <cstdint>

namespace origin
{
static UIEditorPanel *s_Instance = nullptr;

UIEditorPanel::UIEditorPanel(Scene *scene, bool open)
    : m_scene(scene)
{
    s_Instance = this;

    m_is_open = open;

    m_Camera.InitOrthographic(10.0f, 0.1f, 10.0f);
    m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, 1.0f));

    FramebufferSpecification spec;
    spec.attachments =
    {
        FramebufferTextureFormat::RGBA8,
    };

    spec.width = 1280;
    spec.height = 720;

    m_framebuffer = Framebuffer::Create(spec);
}

void UIEditorPanel::SetContext(Scene *scene)
{
    if (m_ui_comp)
        m_ui_comp = nullptr;
    if (m_is_open)
        m_is_open = false;
    m_scene = scene;
}

void UIEditorPanel::SetActive(UIComponent *component)
{
    if (!m_scene)
        return;

    if (!m_is_open)
    {
        m_ui_comp = component;
        m_is_open = true;
        
        m_Camera.SetOrthoScale(m_ui_comp->Height * 1.3f);
        m_Camera.SetOrthoScaleMax(m_ui_comp->Height * 4.0f);

        m_Camera.SetPosition({m_ui_comp->Width / 2.0f, m_ui_comp->Height / 2.0f,1.0f});

        ImGui::SetWindowFocus("UI Editor");
    }
}

void UIEditorPanel::CreateNewText()
{
    if (!m_ui_comp)
        return;

    UIData<TextComponent> component;
    component.Component.TextString = "This is text component";
    std::string defaultName = "Text";
    m_ui_comp->AddComponent<TextComponent>(defaultName, component);
    m_selected_index = static_cast<int>(m_ui_comp->Components.size()) - 1;
}

void UIEditorPanel::CreateNewTexture()
{
    if (!m_ui_comp)
        return;

    UIData<SpriteRenderer2DComponent> component;
    m_ui_comp->AddComponent<SpriteRenderer2DComponent>("Sprite", component);
    m_selected_index = static_cast<int>(m_ui_comp->Components.size()) - 1;
}

bool UIEditorPanel::RenameComponent(int index, const std::string &newName)
{
    if (m_ui_comp)
        return m_ui_comp->RenameComponent(index, newName);
    return false;
}

void UIEditorPanel::Render()
{
    if (!m_is_open)
        return;

    static float left_panel_width = 160.0f;
    static float right_panel_width = 250.0f;
    static float splitter_thickness = 4.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    
    ImGui::Begin("UI Editor", &m_is_open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Get window width
    const f32 window_width = ImGui::GetContentRegionAvail().x;
    
    // ====================
    // LEFT PANEL
    ImGui::BeginChild("Hierarchy", { left_panel_width, 0.0f }, 0);
    {
        if (ImGui::BeginPopupContextWindow("CreateUI", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Text"))
                UIEditorPanel::CreateNewText();
            if (ImGui::MenuItem("Sprite"))
                UIEditorPanel::CreateNewTexture();
            ImGui::EndPopup();
        }

        if (m_ui_comp)
        {
            for (int i = 0; i < m_ui_comp->Components.size(); i++)
            {
                ImGuiTreeNodeFlags flags = (m_selected_index == i ? ImGuiTreeNodeFlags_Selected : 0)
                    | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow
                    | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Leaf;

                if (UIData<TextComponent> *text = m_ui_comp->GetComponent<TextComponent>(m_ui_comp->Components[i]->name))
                {
                    ImGui::PushID(text->name.c_str());

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.5f, 2.0f });
                    bool node_open = ImGui::TreeNodeEx((void *)(u32)(u64)&text, flags, text->name.c_str());
                    ImGui::PopStyleVar();

                    bool is_deleting = false;
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Delete"))
                            is_deleting = true;
                        ImGui::EndPopup();
                    }

                    if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                        m_selected_index = i;

                    if (node_open)
                        ImGui::TreePop();

                    if (is_deleting)
                    {
                        m_ui_comp->RemoveComponent(i);
                        i--;
                    }
                    ImGui::PopID();
                }

                else if (UIData<SpriteRenderer2DComponent> *sprite = m_ui_comp->GetComponent<SpriteRenderer2DComponent>(m_ui_comp->Components[i]->name))
                {
                    ImGui::PushID(sprite->name.c_str());

                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.5f, 2.0f });
                    bool node_open = ImGui::TreeNodeEx((void *)(u32)(u64)&sprite, flags, sprite->name.c_str());
                    ImGui::PopStyleVar();

                    bool isDeleting = false;
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Delete"))
                        {
                            isDeleting = true;
                        }
                        ImGui::EndPopup();
                    }

                    if (ImGui::IsItemHovered())
                    {
                        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                        {
                            m_selected_index = i;
                        }
                    }

                    if (node_open)
                    {
                        ImGui::TreePop();
                    }

                    if (isDeleting)
                    {
                        m_ui_comp->RemoveComponent(i);
                        i--;
                    }

                    ImGui::PopID();
                }
            }
        }
    }
    ImGui::EndChild();
    
    // Left Splitter
    ImGui::SameLine();
    ImGui::InvisibleButton("LeftSplitter", { splitter_thickness, ImGui::GetContentRegionAvail().y });
    if (ImGui::IsItemActive())
        left_panel_width += ImGui::GetIO().MouseDelta.x;

    // ====================
    // VIEWPORT
    ImGui::SameLine();
    const f32 viewport_width = window_width - left_panel_width - right_panel_width - 2 * splitter_thickness;
    ImGui::BeginChild("viewport", { viewport_width, 0.0f }, 0);
    {
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        
        IsViewportFocused = ImGui::IsWindowFocused();
        IsViewportHovered = ImGui::IsWindowHovered();
        const ImVec2 &viewport_min_region = ImGui::GetWindowContentRegionMin();
        const ImVec2 &viewport_max_region = ImGui::GetWindowContentRegionMax();
        const ImVec2 &viewport_offset = ImGui::GetWindowPos();
        m_viewport_rect.min = { viewport_min_region.x + viewport_offset.x, viewport_min_region.y + viewport_offset.y };
        m_viewport_rect.max = { viewport_max_region.x + viewport_offset.x, viewport_max_region.y + viewport_offset.y };
        // Framebuffer Texture
        ImTextureID texture = (void *)(uintptr_t)(m_framebuffer->GetColorAttachment());
        ImGui::Image(texture, { m_viewport_rect.GetSize().x, m_viewport_rect.GetSize().y }, ImVec2(0, 1), ImVec2(1, 0));
        
        std::string zoom_str = fmt::format("Zoom: {}", m_Camera.GetOrthoScale() / m_framebuffer->GetHeight());
        draw_list->AddText({m_viewport_rect.min.x, m_viewport_rect.min.y}, 0xFFFFFFFF, zoom_str.c_str(), nullptr);
    }
    ImGui::EndChild();

    // Right Splitter
    ImGui::SameLine();
    ImGui::InvisibleButton("RightSplitter", { splitter_thickness, ImGui::GetContentRegionAvail().y });
    if (ImGui::IsItemActive())
        right_panel_width -= ImGui::GetIO().MouseDelta.x;

    // ====================
    // RIGHT PANEL
    ImGui::SameLine();
    ImGui::BeginChild("inspector", { right_panel_width, 0.0f }, 0);
    {
        if (m_ui_comp)
        {
            glm::vec2 size = {m_ui_comp->Width, m_ui_comp->Height};
        
            if (UI::DrawVec2Control("Canvas Size", size))
            {
                m_ui_comp->Width = size.x;
                m_ui_comp->Height = size.y;
            }

            for (int i = 0; i < m_ui_comp->Components.size(); i++)
            {
                if (m_selected_index == i)
                {
                    if (UIData<TextComponent> *text = m_ui_comp->GetComponent<TextComponent>(m_ui_comp->Components[i]->name))
                    {
                        std::string name = text->name;

                        char buffer[256];
                        strncpy(buffer, name.c_str(), sizeof(buffer));
                        if (ImGui::InputText("##Tag", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
                        {
                            std::string new_name(buffer);
                            if (!new_name.empty() && new_name != name)
                            {
                                m_ui_comp->RenameComponent(m_selected_index, new_name);
                            }
                        }

                        UI::DrawVec2Control("Offset", text->offset);
                        UI::DrawVec2Control("Min", text->rect.min);
                        UI::DrawVec2Control("Max", text->rect.max);
                        UI::DrawFloatControl("Rotation", &text->rotation, 1.0f, 0.0f, 360.0f, 0.0f);
                    
                        UI::DrawButtonWithColumn("Font", "Drag Here", nullptr, [&]()
                        {
                            if (ImGui::BeginDragDropTarget())
                            {
                                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                                {
                                    AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
                                    if (AssetManager::GetAssetType(handle) == AssetType::Font)
                                    {
                                        text->Component.FontHandle = handle;
                                    }
                                    else
                                    {
                                        OGN_CORE_WARN("[UIEditor] Wrong asset type!");
                                        PUSH_CONSOLE_WARNING("[UIEditor] Wrong asset type!");
                                    }
                                }
                                ImGui::EndDragDropTarget();
                            }

                            if (text->Component.FontHandle)
                            {
                                ImGui::SameLine();
                                if (UI::DrawButton("X"))
                                {
                                    text->Component.FontHandle = 0;
                                }
                            }
                        });


                        if (text->Component.FontHandle)
                        {
                            ImGui::InputTextMultiline("Text String", &text->Component.TextString);
                            ImGui::ColorEdit4("Color", glm::value_ptr(text->Component.Color));
                            UI::DrawFloatControl("Kerning", &text->Component.Kerning, 0.01f);
                            UI::DrawFloatControl("Line Spacing", &text->Component.LineSpacing, 0.01f);
                        }
                    }
                    else if (UIData<SpriteRenderer2DComponent> *sprite = m_ui_comp->GetComponent<SpriteRenderer2DComponent>(m_ui_comp->Components[i]->name))
                    {
                        std::string name = sprite->name;

                        char buffer[256];
                        strncpy(buffer, name.c_str(), sizeof(buffer));
                        if (ImGui::InputText("##Tag", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
                        {
                            std::string newName(buffer);
                            if (!newName.empty() && newName != name)
                            {
                                m_ui_comp->RenameComponent(m_selected_index, newName);
                            }
                        }

                        UI::DrawVec2Control("Offset", sprite->offset);
                        UI::DrawVec2Control("Min", sprite->rect.min);
                        UI::DrawVec2Control("Max", sprite->rect.max);
                        UI::DrawFloatControl("Rotation", &sprite->rotation);
                    
                        ImGui::ColorEdit4("Color", glm::value_ptr(sprite->Component.Color));

                        std::string label = "None";
                        if (sprite->Component.Texture != 0)
                        {
                            if (AssetManager::IsAssetHandleValid(sprite->Component.Texture) && AssetManager::GetAssetType(sprite->Component.Texture) == AssetType::Texture)
                            {
                                const AssetMetadata &metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(sprite->Component.Texture);
                                label = metadata.Filepath.filename().string();
                            }
                            else
                            {
                                label = "Invalid";
                            }
                        }

                        UI::DrawButtonWithColumn(label.c_str(), "Texture", nullptr, [&]()
                        {
                            if (ImGui::BeginDragDropTarget())
                            {
                                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
                                {
                                    AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
                                    if (AssetManager::GetAssetType(handle) == AssetType::Texture)
                                    {
                                        sprite->Component.Texture = handle;
                                        sprite->Component.UV0 = glm::vec2(0.0f);
                                        sprite->Component.UV1 = glm::vec2(1.0f);
                                    }
                                    else
                                    {
                                        OGN_CORE_WARN("[UIEditor] Wrong asset type!");
                                        PUSH_CONSOLE_WARNING("[UIEditor] Wrong asset type!");
                                    }
                                }
                                else if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SPRITESHEET_ITEM"))
                                {
                                    SpriteSheetData data = *static_cast<SpriteSheetData *>(payload->Data);
                                    sprite->Component.Texture = data.texture_handle;
                                    sprite->Component.UV0 = data.rect.min;
                                    sprite->Component.UV1 = data.rect.max;
                                }

                                ImGui::EndDragDropTarget();
                            }
                            if (sprite->Component.Texture)
                            {
                                ImGui::SameLine();
                                if (UI::DrawButton("X"))
                                {
                                    sprite->Component.Texture = 0;
                                    sprite->Component.UV0 = glm::vec2(0.0f);
                                    sprite->Component.UV1 = glm::vec2(1.0f);
                                }
                            }
                        });

                        if (sprite->Component.Texture)
                        {
                            UI::DrawVec2Control("Tilling Factor", sprite->Component.TillingFactor, 0.025f, 1.0f);
                            UI::DrawCheckbox("Flip X", &sprite->Component.FlipX);
                            UI::DrawCheckbox("Flip Y", &sprite->Component.FlipY);
                        }
                    }
                }
            }
        }
    }
    ImGui::EndChild();

    ImGui::End();

    ImGui::PopStyleVar();
}
    
void UIEditorPanel::OnUpdate(f32 delta_time)
{
    if (!m_is_open)
    {
        if (m_ui_comp)
            m_ui_comp = nullptr;
        if (m_scene)
            m_scene = nullptr;
        return;
    }

    if (IsViewportHovered && IsViewportFocused)
        m_Camera.OnUpdate(delta_time);

    m_Camera.UpdateView();
    m_Camera.UpdateProjection();

    OnMouse(delta_time);

    m_framebuffer->Bind();
    RenderCommand::Clear();
    RenderCommand::ClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    m_framebuffer->ClearAttachment(1, -1);

    if (m_ui_comp != nullptr)
    {
        const auto &vp_size = m_viewport_rect.GetSize();
        if (const FramebufferSpecification spec = m_framebuffer->GetSpecification(); vp_size.x > 0.0f && vp_size.y > 0.0f && (vp_size.x != spec.width || vp_size.y != spec.height))
        {
            m_Camera.SetViewportSize(static_cast<u32>(vp_size.x), static_cast<u32>(vp_size.y));
            m_framebuffer->Resize(static_cast<u32>(vp_size.x), static_cast<u32>(vp_size.y));
        }

        Renderer::camera_uniform_buffer->Bind();
        glm::mat4 view_projection = m_Camera.GetViewProjection();
        glm::vec3 cam_position = m_Camera.GetPosition();
        Renderer::camera_uniform_buffer->SetData(&view_projection, sizeof(CameraBufferData), 0);
        Renderer::camera_uniform_buffer->SetData(&cam_position, sizeof(CameraBufferData), sizeof(glm::mat4));
        
        Renderer2D::Begin();
        const Rect ui_rect = {0.0f, 0.0f, m_ui_comp->Width, m_ui_comp->Height};
        Renderer2D::DrawRect(ui_rect, {0.5f, 0.5f, 0.5f, 1.0f});

        for (int i = 0; i < m_ui_comp->Components.size(); i++)
        {
            if (UIData<TextComponent> *text = m_ui_comp->GetComponent<TextComponent>(m_ui_comp->Components[i]->name))
            {
                if (text->Component.FontHandle)
                {
                    Renderer2D::DrawString(text->Component.TextString, text->rect, text->Component, text->anchor_type, text->rotation, text->offset);
                }
            }
            else if (UIData<SpriteRenderer2DComponent> *sprite = m_ui_comp->GetComponent<SpriteRenderer2DComponent>(m_ui_comp->Components[i]->name))
            {
                Renderer2D::DrawSprite(sprite->rect, sprite->Component, sprite->anchor_type, sprite->rotation, sprite->offset);
                if (m_selected_index == i)
                {
                    RenderCommand::SetLineWidth(2.0f);
                    Renderer2D::DrawRect(sprite->rect, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f), sprite->offset);
                }
            }
        }

        Renderer2D::End();
        RenderCommand::SetLineWidth(1.0f);
    }

    m_framebuffer->Unbind();
}

void UIEditorPanel::OnEvent(Event &e)
{
    OGN_PROFILER_INPUT();

    m_Camera.OnEvent(e);
    EventDispatcher dispatcher(e);

    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(UIEditorPanel::OnMouseButtonPressed));
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(UIEditorPanel::OnKeyPressed));
    dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(UIEditorPanel::OnMouseScroll));
    dispatcher.Dispatch<MouseMovedEvent>(OGN_BIND_EVENT_FN(UIEditorPanel::OnMouseMoved));
}

bool UIEditorPanel::OnMouseButtonPressed(MouseButtonPressedEvent &e)
{
    if (IsViewportHovered && !IsViewportFocused)
    {
        if (e.Is(Mouse::ButtonMiddle) || e.Is(Mouse::ButtonRight) || e.Is(Mouse::ButtonLeft))
        {
            ImGui::SetWindowFocus("UI Editor");
        }
    }

    if (e.Is(Mouse::ButtonLeft) && IsViewportHovered)
    {
        if (m_hovered_index != (m_selected_index == 0 ? -1 : m_selected_index) && m_hovered_index >= 0)
        {
            m_selected_index = m_hovered_index;
        }
        else if (m_hovered_index <= -1)
        {
            m_selected_index = -1;
        }
    }

    return false;
}

bool UIEditorPanel::OnMouseScroll(MouseScrolledEvent &e)
{
    if (IsViewportHovered)
        m_Camera.OnMouseScroll(e.GetYOffset());

    return false;
}

bool UIEditorPanel::OnKeyPressed(KeyPressedEvent &e)
{
    return false;
}

inline bool UIEditorPanel::OnMouseMoved(MouseMovedEvent& e)
{
    if (IsViewportHovered && IsViewportFocused)
    {
        const glm::vec2 &mouse_delta = Input::GetMouseClickDragDelta();
        m_Camera.OnMouseMove(mouse_delta);
    }
    
    return false;
}

void UIEditorPanel::OnMouse(f32 ts)
{
    const glm::vec2 delta = Input::GetMouseClickDragDelta();

    if (!m_ui_comp)
        return;

    if (Input::IsMouseButtonPressed(Mouse::ButtonLeft) && IsViewportHovered)
    {
        for (int i = 0; i < m_ui_comp->Components.size(); i++)
        {
            if (m_selected_index == i)
            {
                f32 viewport_height = m_Camera.GetViewportSize().y;
                f32 orthoScale = m_Camera.GetOrthoScale() / viewport_height;

                static glm::vec2 translation = m_ui_comp->Components[m_selected_index]->rect.GetCenter();

                if (Input::IsKeyModPressed((KeyMod::LeftShift)))
                {
                    f32 snap_value = 0.5f;
                    if (Input::IsKeyModPressed(KeyMod::LeftControl))
                        snap_value = 0.1f;

                    translation += glm::vec2(-delta.x * orthoScale, delta.y * orthoScale);
                    m_ui_comp->Components[m_selected_index]->rect += translation * snap_value;
                }
                else
                {
                    translation = glm::vec2(-delta.x * orthoScale, delta.y * orthoScale);
                    m_ui_comp->Components[m_selected_index]->rect += translation;
                }
            }
        }
    }
}

void UIEditorPanel::Open()
{
    m_is_open = true;
    ImGui::SetWindowFocus("UI Editor");
}

void UIEditorPanel::Close()
{
    m_is_open = false;
    Reset();
}

void UIEditorPanel::Reset()
{
    m_ui_comp = nullptr;
}

UIEditorPanel *UIEditorPanel::GetInstance()
{
    return s_Instance;
}

}
