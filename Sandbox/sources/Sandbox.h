#pragma once
#include <Origin.h>

namespace Origin {

  class Sandbox : public Layer
  {
  public:
    Sandbox();
    ~Sandbox();

    void OnAttach() override;
    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& event) override;

    void OnGuiRender() override;

    bool OnWindowResize(WindowResizeEvent& e);

    EditorCamera camera;
  };

}