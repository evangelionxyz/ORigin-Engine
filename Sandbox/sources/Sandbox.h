#pragma once
#include <Origin.h>
#include "Draw.h"

#include <glm\glm.hpp>

#include "FPSCamera.h"

using namespace Origin;

class Sandbox : public Layer
{
public:
  Sandbox();
  ~Sandbox();

  void OnUpdate(Timestep ts) override;
  void OnEvent(Event& event) override;

  void OnGuiRender() override;
  bool OnWindowResize(WindowResizeEvent& e);

  FPSCamera camera;

  std::shared_ptr<Texture2D> cubeTexture;
  glm::vec3 cubePosition = glm::vec3(0.0f, -1.0f, 0.0f);
  glm::vec3 cubeSize = glm::vec3(2.0f, 0.0f, 2.0f);
  glm::vec4 cubeColor = glm::vec4(1.0f);

	glm::vec3 lightPosition = glm::vec3(0.0f, 2.0f, 0.0f);
  glm::vec3 lightSize = glm::vec3(0.5f);
	glm::vec4 lightColor = glm::vec4(1.0f);
  float Ambient = 0.8f;

private:
  Draw draw;
};