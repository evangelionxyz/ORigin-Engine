// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef SPRITE_SHEET_EDITOR_HPP
#define SPRITE_SHEET_EDITOR_HPP

#include "Origin/Core/Event.h"
#include "Origin/Core/KeyEvent.h"
#include "Origin/Scene/SpriteSheet.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/Framebuffer.h"
#include "Origin/Scene/Camera/EditorCamera.h"
#include "PanelBase.hpp"

#include "Origin/Math/Math.hpp"

#include <glm/glm.hpp>

namespace origin {
class SpriteSheet;

enum ControllerCorner : u8
{
    NONE = 0,
    TOP_LEFT,
    BOTTOM_LEFT,
    TOP_RIGHT,
    BOTTOM_RIGHT
};

struct SpriteSheetController
{
    struct Corner
    {
        Rect top_left;
        Rect top_right;
        Rect bottom_left;
        Rect bottom_right;
    };

    SpriteSheetData data;
    Corner corner;
    ControllerCorner selected_corner = ControllerCorner::NONE;
};

class SpriteSheetEditorPanel : public PanelBase
{
public:
    SpriteSheetEditorPanel();

    Ref<SpriteSheet> CreateSpriteSheet(const AssetHandle texture_handle);

    void OpenSpriteSheet(const AssetHandle sprite_sheet_handle);

    void RemoveSprite(i32 index);
    void Duplicate(i32 index);

    void Render() override;
    void OnUpdate(float delta_time) override;

    bool Serialize(const std::filesystem::path &filepath);
    bool LoadSpritesToController();

    void OnEvent(Event &e) override;
    bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
    bool OnKeyPressed(const KeyPressedEvent &e);
    bool OnMouseScroll(const MouseScrolledEvent &e);
    void OnMouse(f32 ts);

    static SpriteSheetEditorPanel *GetInstance();

    bool IsViewportFocused = false;
    bool IsViewportHovered = false;
private:
    glm::vec2 grid_size;
    EditorCamera m_Camera;
    Ref<SpriteSheet> m_SpriteSheet;
    Ref<Framebuffer> m_Framebuffer;
    Ref<Texture2D> m_Texture;
    std::vector<SpriteSheetController> m_Controls;
    std::filesystem::path m_CurrentFilepath;
    i32 m_SelectedIndex = 0;
};
}

#endif

