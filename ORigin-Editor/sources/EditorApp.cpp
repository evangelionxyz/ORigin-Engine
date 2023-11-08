// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include <Origin.h>
#include "EditorLayer.h"

namespace origin
{
  class EditorApp : public Application
  {
  public:
    EditorApp(const ApplicationSpecification& spec)
      : Application(spec)
    {
      Application::Get().GetWindow().SetIcon("Resources/UITextures/icon_origin.png");
	    PushLayer(new EditorLayer());
    }

    ~EditorApp()
    {
    }
  };

  Application* CreateApplication(ApplicationCommandLineArgs args)
  {
    ApplicationSpecification spec;
    spec.Name = "ORigin-Editor";
    spec.CommandLineArgs = args;

    OGN_CORE_INFO(spec.Name);
    return new EditorApp(spec);
  };
}