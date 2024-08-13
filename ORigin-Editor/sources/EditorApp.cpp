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
        spec.Width = 1640;
        spec.Height = 860;
        spec.Name = "Editor";
        spec.CommandLineArgs = args;
        spec.Maximize = false;

        OGN_CORE_INFO(spec.Name);
        return new EditorApp(spec);
    };
}