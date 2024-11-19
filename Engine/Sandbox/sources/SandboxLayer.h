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
        static u32 LoadTexture(const unsigned char *data, int width, int height);
        std::vector<u32> m_Textures;
        EditorCamera camera;
    };
}
