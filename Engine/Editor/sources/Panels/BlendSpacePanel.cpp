#include "BlendSpacePanel.hpp"

namespace origin {
BlendSpacePanel::BlendSpacePanel()
{
}

BlendSpacePanel::~BlendSpacePanel()
{
}

void BlendSpacePanel::AddAnimation(SkeletalAnimation* animation)
{
    m_animations.push_back(animation);
}

void BlendSpacePanel::Render()
{
    if (!m_is_open)
        return;
    
    ImGui::Begin("Blend space panel");
    ImGui::Text("Blend space");

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    
    ImGui::End();
}

void BlendSpacePanel::OnUpdate(f32 delta_time)
{
    
}

void BlendSpacePanel::OnEvent(Event& e)
{
    
}

}
