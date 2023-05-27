// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "Dockspace.h"

#include "Origin\Scene\SceneSerializer.h"

#include <imgui.h>

namespace Origin
{
  void Dockspace::Begin()
  {
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui::PopStyleVar(2);

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
      window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(1, 2));

    ImGui::Begin("ORigin", nullptr, window_flags);

    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    const float minWindowSizeX = style.WindowMinSize.x;
    const float minWindowSizeY = style.WindowMinSize.y;
    //style.WindowMinSize.x = 220.0f;
    //style.WindowMinSize.y = 120.0f;

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
      const ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    style.WindowMinSize.x = minWindowSizeX;
    style.WindowMinSize.y = minWindowSizeY;
  }

  void Dockspace::End()
  {
    ImGui::End();
    ImGui::PopStyleVar();
  }
}