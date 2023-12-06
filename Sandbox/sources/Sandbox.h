#pragma once
#include <Origin.h>

namespace origin {

  class Sandbox : public Layer
  {
  public:
    Sandbox();
    ~Sandbox();

    void OnUpdate(Timestep ts) override;
    void OnGuiRender() override;

    void OnEvent(Event& event) override;
    bool OnWindowResize(WindowResizeEvent& e);

  private:
    EditorCamera camera;
  };

}
