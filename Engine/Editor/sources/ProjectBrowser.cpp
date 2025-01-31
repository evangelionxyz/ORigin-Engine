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
    if (const auto &cmdline_args = Application::GetInstance().GetSpecification().CommandLineArgs; cmdline_args.Count > 1)
    {
        Application::GetInstance().SubmitToMainThread([this, cmdline_args]()
        {
            EditorLayer *editor_layer = new EditorLayer();
            Application::GetInstance().PushLayer(editor_layer);

            editor_layer->OpenProject(cmdline_args[1]);

            Application::GetInstance().GetWindow().Maximize();
            Application::GetInstance().PopLayer(this);
        });
    }

    RenderCommand::Clear();
}

void ProjectBrowser::OnGuiRender()
{
    Dockspace::Begin();
    ImGui::Begin("Project Browser", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    const ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    const ImVec2 canvas_pos = ImGui::GetWindowPos();
    
    constexpr ImVec2 button_size = ImVec2(120.0f, 30.0f);
    const ImVec2 button_pos = ImVec2(canvas_size.x / 2.0f - button_size.x * 1.5f, canvas_size.y - 50.0f);

    ImGui::SetCursorPos(button_pos);
    if (ImGui::Button("New Project", button_size))
    {
        Application::GetInstance().SubmitToMainThread([this]()
        {
            EditorLayer *editor_layer = new EditorLayer();
            Application::GetInstance().PushLayer(editor_layer);
            editor_layer->NewProject();

            Application::GetInstance().GetWindow().Maximize();
            Application::GetInstance().PopLayer(this);
        });
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Open", button_size))
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
    if (ImGui::Button("Cancel", button_size))
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
