#pragma once
#include <Origin.h>
#include "Lighting.h"
#include "HDRScreen.h"

namespace origin {

  class SandboxLayer : public Layer
  {
  public:
    SandboxLayer();
    ~SandboxLayer();

    void OnUpdate(Timestep ts) override;
    void OnGuiRender() override;
    void OnEvent(Event& event) override;
    bool OnWindowResize(WindowResizeEvent& e);

  private:
    EditorCamera camera;
    TestLight* light = nullptr;
    HDRScreen* screen = nullptr;
  };

}
