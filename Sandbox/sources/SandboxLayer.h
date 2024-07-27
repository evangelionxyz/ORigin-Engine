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

    private:
        void DrawGrid();
        EditorCamera m_Camera;
        SceneCamera m_SceneCamera;
        TransformComponent m_CamTC;
        Frustum m_Frustum;

        const int size = 20;
        bool polygonMode = false;
        float nPlane, fPlane, FOV;
        float deltaTime = 0.0f;
        float updateRate = 1.0f;
        float timer = 1.0f;
        float response = 0.0f;
        float framerate = 0.0f;
    };
}
