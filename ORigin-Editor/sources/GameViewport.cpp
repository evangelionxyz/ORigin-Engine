// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "Origin/Scene/Component.h"
#include "Editor.h"

namespace origin
{
    void Editor::GameViewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar
            | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoCollapse;

        ImGui::Begin("Game", nullptr, window_flags);
        
        const ImVec2& viewportMinRegion = ImGui::GetWindowContentRegionMin();
        const ImVec2& viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        const ImVec2& viewportOffset = ImGui::GetWindowPos();
        m_GameViewportBounds[0] = {viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
        m_GameViewportBounds[1] = {viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

        const ImVec2& viewportPanelSize = ImGui::GetContentRegionAvail();
        m_GameViewportSize = {viewportPanelSize.x, viewportPanelSize.y};

        const auto viewportID = reinterpret_cast<ImTextureID>(m_GameFramebuffer->GetColorAttachmentRendererID(m_RenderTarget));
        ImGui::Image(viewportID, ImVec2(m_GameViewportSize.x, m_GameViewportSize.y), ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();
    }
}
