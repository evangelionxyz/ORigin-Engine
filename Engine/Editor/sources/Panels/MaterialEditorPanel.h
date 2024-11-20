// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Renderer/Material.h"
#include "Origin/Asset/Asset.h"

#include "PanelBase.h"

#include "Origin/Core/MouseEvent.h"
#include "Origin/Core/KeyEvent.h"

#include <filesystem>

namespace origin {
class MaterialEditorPanel : public PanelBase
{
public:
    MaterialEditorPanel();

    void SetSelectedMaterial(AssetHandle handle);
    void Render() override;
    void OnUpdate(float delta_time) override;

    void OnEvent(Event &e) override;

    bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
    bool OnKeyPressed(KeyPressedEvent &e);
    bool Deserialize();

    static MaterialEditorPanel *GetInstance();

private:
    std::filesystem::path m_CurrentFilepath;
    std::shared_ptr<Material> m_Material;
};

}
