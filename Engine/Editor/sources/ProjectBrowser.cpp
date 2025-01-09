// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "ProjectBrowser.hpp"
#include "EditorApp.hpp"
#include "EditorLayer.hpp"

#include "Dockspace.hpp"
#include "imgui.h"
#include "Origin/Renderer/Renderer.h"
#include "Origin/Utils/PlatformUtils.h"

namespace origin {

void ProjectBrowser::OnAttach()
{
}

void ProjectBrowser::OnDetach()
{
}

void ProjectBrowser::OnUpdate(Timestep ts)
{
    RenderCommand::Clear();
}

void ProjectBrowser::OnGuiRender()
{
    Dockspace::Begin();
    ImGui::Begin("Project Browser", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    if (ImGui::Button("Open"))
    {
        Application::GetInstance().SubmitToMainThread([this]()
         {
             const std::filesystem::path project_filepath = FileDialogs::OpenFile("Origin Project (*.oxproj)\0*.oxproj\0");
             if (!project_filepath.empty())
             {
                 EditorLayer *editor_layer = new EditorLayer();
                 Application::GetInstance().PushLayer(editor_layer);

                 editor_layer->OpenProject(project_filepath);

                 Application::GetInstance().GetWindow().Maximize();
                
                 Application::GetInstance().PopLayer(this);
             }
         });
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel"))
    {
        Application::GetInstance().Close();
    }
    
    ImGui::End();
    Dockspace::End();
}

void ProjectBrowser::OnEvent(Event& e)
{
}

}
