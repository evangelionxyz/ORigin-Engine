#pragma once
#include <Origin.h>

#include "Frustum.h"

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

    private:
        void DrawGrid();
        glm::vec3 GetRandomColor();

        EditorCamera m_Camera;
        SceneCamera m_SceneCamera;
        TransformComponent m_CamTC;
        Frustum m_Frustum;

        std::mt19937 rng;
        std::uniform_real_distribution<float> dist;
        std::vector<glm::vec3> randomColor;
        bool polygonMode = false;
        float nPlane, fPlane, FOV;
        int size = 20;
        float timeColorChange = 0.0f;
        float deltaTime = 0.0f;
        float updateRate = 1.0f;
        float timer = 1.0f;
        float response = 0.0f;
        float framerate = 0.0f;
    };
}
