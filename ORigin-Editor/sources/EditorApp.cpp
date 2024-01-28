// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include <Origin.h>
#include "EditorLayer.h"

namespace origin
{
  class RuntimeApp : public Application
  {
  public:
    RuntimeApp(const ApplicationSpecification& spec)
      : Application(spec)
    {
	    PushLayer(new EditorLayer());
    }

    ~RuntimeApp()
    {}
  };

  Application* CreateApplication(ApplicationCommandLineArgs args)
  {
    ApplicationSpecification spec;
    spec.Name = "ORigin Editor";
    spec.CommandLineArgs = args;
    spec.Maximize = true;

    OGN_CORE_INFO(spec.Name);
    return new RuntimeApp(spec);
  };
}