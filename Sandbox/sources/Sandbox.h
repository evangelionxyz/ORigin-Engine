#pragma once
#include <Origin.h>

namespace Origin {

  class Sandbox : public Layer
  {
  public:
    Sandbox() : Layer("Sandbox")
    {
    }

    void OnAttach() override;

    void OnUpdate(Timestep ts) override;
    void OnEvent(Event& event) override;

  private:
    ShaderLibrary m_ShaderLibrary;
    OrthoCameraController m_CameraController;

    std::shared_ptr<Texture2D> m_Texture, m_Sprites, m_Logo;

    std::shared_ptr<SubTexture2D> m_StairsTexture;
    std::shared_ptr<SubTexture2D> m_BarrelTexture;
    std::shared_ptr<SubTexture2D> m_TreeTexture;

    glm::vec4 color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);

    float incremental = 20.0f;
    float rotation = 0.0f;
    float rotationSpeed = 50.0f;

  private:
  };

}