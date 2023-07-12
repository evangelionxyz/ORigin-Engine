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
    std::shared_ptr<Animation> marioAnim, fireballAnim;
    std::shared_ptr<Texture2D> mario1, mario2, mario3, mario4, mario5;
    std::shared_ptr<Texture2D> fireball1, fireball2, fireball3, fireball4, fireball5, fireball6, fireball7, fireball8;

    AnimationState AnimState;

    glm::vec3 marioPosition = glm::vec3(-1.0f, 0.0f, 0.0f);
    glm::vec3 fireballPosition = glm::vec3(1.0f, 0.0f, 0.0f);

    bool switching = false;

    float marioFrameTime = 0.1f;
    float fireballFrameTime = 0.3f;
    bool looping = true, fireballLooping = false;
  };

}
