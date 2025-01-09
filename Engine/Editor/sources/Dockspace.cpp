// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Dockspace.hpp"

#include <imgui.h>

namespace origin {
void Dockspace::Begin()
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 2));
    
    ImGui::Begin("Origin", nullptr, window_flags);

    ImGuiStyle &style = ImGui::GetStyle();
    const ImVec2 min_window_size = style.WindowMinSize;
    style.WindowMinSize.x = 220.0f;
    style.WindowMinSize.y = 26.0f;

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        const ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        style.WindowMinSize = min_window_size;
    }
    
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);
}

void Dockspace::End()
{
    ImGui::End();
}
}