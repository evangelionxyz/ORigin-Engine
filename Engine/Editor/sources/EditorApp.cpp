// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include <Origin.h>
#include "EditorLayer.h"

namespace origin
{
    class EditorApp : public Application
    {
    public:
        EditorApp(ApplicationSpecification &spec)
            : Application(spec)
        {
            PushLayer(new EditorLayer());
        }
    };

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