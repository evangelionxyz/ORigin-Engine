// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include <Origin.h>
#include "EditorLayer.h"

namespace origin
{
  class EditorApp : public Application
  {
  public:
    EditorApp(ApplicationSpecification& spec)
      : Application(spec)
    {
	    PushLayer(new EditorLayer());
    }

    ~EditorApp() {}
  };

  Application* CreateApplication(ApplicationCommandLineArgs args)
  {
    ApplicationSpecification spec;
    spec.Name = "ORigin Editor";
    spec.CommandLineArgs = args;
    spec.Maximize = false;

    OGN_CORE_INFO(spec.Name);
    return new EditorApp(spec);
  };
}