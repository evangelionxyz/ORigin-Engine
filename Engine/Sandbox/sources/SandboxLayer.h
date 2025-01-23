#pragma once
#include <Origin.hpp>

#include <random>
#include <chrono>

namespace origin
{
    class SandboxLayer : public Layer
    {
    public:
        SandboxLayer();
        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(Timestep ts) override;
        void OnGuiRender() override;
        void OnEvent(Event &event) override;
        bool OnWindowResize(FramebufferResizeEvent &e);
        bool OnKeyPressed(KeyPressedEvent &e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
        bool OnMouseMove(MouseMovedEvent &e);
        bool OnMouseScroll(MouseScrolledEvent &e);

        EditorCamera camera;
    };
}
