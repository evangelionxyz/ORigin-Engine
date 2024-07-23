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
        ~SandboxLayer();

        void OnAttach() override;
        void OnUpdate(Timestep ts) override;
        void OnGuiRender() override;
        void OnEvent(Event &event) override;

        bool OnWindowResize(FramebufferResizeEvent &e);
        bool OnKeyPressedEvent(KeyPressedEvent &e);
        void DrawGrid();

        EditorCamera m_Camera;

        bool polygonMode = false;
        int size = 50;

        float timeColorChange = 0.0f;

        std::mt19937 rng;
        std::uniform_real_distribution<float> dist;
        std::vector<glm::vec3> randomColor;

        glm::vec3 GetRandomColor();
    };
}
