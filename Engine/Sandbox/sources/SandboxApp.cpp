// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include <Origin.hpp>
#include "SandboxLayer.h"

namespace origin
{
    class SandboxApp : public Application
    {
    public:
        SandboxApp(ApplicationSpecification &spec)
            : Application(spec)
        {
            PushLayer(new SandboxLayer());
        }
    };

    Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        ApplicationSpecification spec;
        spec.Name = "Sandbox";
        spec.Width = 1280;
        spec.Height = 720;
        spec.Maximize = true;
        spec.WorkingDirectory = "../Editor";
        spec.CommandLineArgs = args;

        OGN_CORE_INFO(spec.Name.c_str());
        return new SandboxApp(spec);
    };
}