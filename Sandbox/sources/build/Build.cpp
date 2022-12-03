// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
#include <Origin.h>
#include "..\Sandbox.h"

namespace Origin {

  class EditorBuild
    : public Application
  {
  public:
    EditorBuild(ApplicationCommandLineArgs args)
      : Application("ORigin-Editor", args)
    {
      Application::Get().GetWindow().SetIcon("assets/textures/icon_origin.png");
      PushLayer(new Editor());
    }
    ~EditorBuild() { }
  };

  Application* CreateApplication(ApplicationCommandLineArgs args)
  {
    OGN_CORE_INFO("ORigin-Editor");
    return new EditorBuild(args);
  };
}