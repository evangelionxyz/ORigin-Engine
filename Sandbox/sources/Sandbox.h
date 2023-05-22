#pragma once
#include <Origin.h>

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
};