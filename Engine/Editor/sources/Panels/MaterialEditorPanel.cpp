#include "MaterialEditorPanel.hpp"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Profiler/Profiler.h"
#include "Origin/GUI/UI.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace origin {

MaterialEditorPanel *s_Instance = nullptr;

MaterialEditorPanel::MaterialEditorPanel()
{
    s_Instance = this;
}

void MaterialEditorPanel::SetSelectedMaterial(AssetHandle handle)
{
}

void MaterialEditorPanel::Render()
{
}

void MaterialEditorPanel::OnUpdate(float delta_time)
{

}

void MaterialEditorPanel::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseButtonPressedEvent>(OGN_BIND_EVENT_FN(MaterialEditorPanel::OnMouseButtonPressed));
    dispatcher.Dispatch<KeyPressedEvent>(OGN_BIND_EVENT_FN(MaterialEditorPanel::OnKeyPressed));
}

bool MaterialEditorPanel::OnMouseButtonPressed(MouseButtonPressedEvent &e)
{
    return false;
}

bool MaterialEditorPanel::OnKeyPressed(KeyPressedEvent &e)
{
    return false;
}

bool MaterialEditorPanel::Deserialize()
{
    return false;
}

MaterialEditorPanel *MaterialEditorPanel::GetInstance()
{
    return s_Instance;
}

}
