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
    Animation anim;
    std::shared_ptr<Texture2D> tex1, tex2, tex3, tex4, tex5;
    float frameTime = 0.23f;
    bool looping = true;
  };

}
