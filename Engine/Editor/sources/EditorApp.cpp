// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "EditorApp.hpp"

#include "ProjectBrowser.hpp"
#include "Themes.hpp"

namespace origin {
EditorApp::EditorApp(ApplicationSpecification &spec)
        : Application(spec)
{
    Themes::Init();
    Themes::ApplyTheme("Dark");
    ProjectBrowser *project_browser = new ProjectBrowser();
    PushLayer(project_browser);
}

Application *CreateApplication(ApplicationCommandLineArgs args)
{
    ApplicationSpecification spec;
    spec.Width = 1280;
    spec.Height = 640;
    spec.Name = "Editor";
    spec.CommandLineArgs = args;
    spec.Maximize = false;

    OGN_CORE_INFO(spec.Name.c_str());
    return new EditorApp(spec);
};
}
