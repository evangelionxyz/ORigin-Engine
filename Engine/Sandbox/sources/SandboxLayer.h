#pragma once
#include <Origin.h>

#include <random>
#include <chrono>

namespace origin
{
    class SandboxLayer : public Layer
    {
    public:
        SandboxLayer();
        void OnAttach() override;
        void OnUpdate(Timestep ts) override;
        void OnGuiRender() override;
        void OnEvent(Event &event) override;
        bool OnWindowResize(FramebufferResizeEvent &e);
        bool OnKeyPressedEvent(KeyPressedEvent &e);
        EditorCamera camera;
    };
}
