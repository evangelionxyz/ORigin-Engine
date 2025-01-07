// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SPRITE_SHEET_EDITOR_H
#define SPRITE_SHEET_EDITOR_H

#include "Origin/Core/Event.h"
#include "Origin/Core/KeyEvent.h"
#include "Origin/Scene/SpriteSheet.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/Framebuffer.h"
#include "Origin/Scene/EditorCamera.h"
#include "PanelBase.h"

#include "Origin/Math/Math.h"

#include <glm/glm.hpp>

namespace origin
{
    class SpriteSheet;

    enum ControllerCorner
    {
        NONE = -1,
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

        Rect rect = Rect({ 1.0f, 1.0f }, { 1.0f, 1.0f });
        Corner corner;
        ControllerCorner selected_corner = ControllerCorner::NONE;
    };

    class SpriteSheetEditorPanel : public PanelBase
    {
    public:
        SpriteSheetEditorPanel();

        void CreateNewSpriteSheet();
        void SetSelectedSpriteSheet(AssetHandle handle);
        void SetMainTexture(AssetHandle handle) const;

        void AddSprite(const glm::vec2 &position, const glm::vec2 &size, const Rect &rect) const;
        void RemoveSprite(i32 index);
        void Duplicate(i32 index);

        void Render() override;
        void OnUpdate(float delta_time) override;

        bool Serialize(const std::filesystem::path &filepath);
        bool Deserialize();

        void OnEvent(Event &e) override;
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
        bool OnKeyPressed(const KeyPressedEvent &e);
        bool OnMouseScroll(const MouseScrolledEvent &e);
        void OnMouse(f32 ts);

        static SpriteSheetEditorPanel *GetInstance();

        bool IsViewportFocused = false;
        bool IsViewportHovered = false;
    private:

        void Reset();

        EditorCamera m_Camera;
        std::shared_ptr<SpriteSheet> m_SpriteSheet;
        std::shared_ptr<Framebuffer> m_Framebuffer;
        std::shared_ptr<Texture2D> m_Texture;
        std::vector<SpriteSheetController> m_Controls;

        glm::vec2 m_MoveTranslation = { 0.0f, 0.0f };
        std::filesystem::path m_CurrentFilepath;
        i32 m_SelectedIndex = 0;
    };

}

#endif

