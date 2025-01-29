#pragma once
#include <Origin.hpp>

#include <random>
#include <chrono>

#include "Dockspace.hpp"

namespace origin {
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
    bool OnMouseScroll(MouseScrolledEvent &e);

private:
    void UpdateCamera(f32 delta_time);
    void UpdateCharacterMovement(f32 delta_time);
    void RenderScene(f32 delta_time);
    void RenderSkybox(f32 delta_time);
    void InitSounds();
    bool viewport_hovered = false;

    EditorCamera camera;
    Ref<Framebuffer> main_framebuffer;
    Ref<FmodSound> roar_sound;
    Ref<Skybox> skybox;
    Ref<Shader> shader;            // base shader
    Ref<Shader> skybox_shader;     // skybox shader
    Ref<DirectionalLight> dir_light;
    glm::vec4 tint_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec4 grid_thin_color = { 0.8f, 0.8f , 0.8f, 1.0f };
    glm::vec4 grid_thick_color = { 0.5f, 0.5f, 0.5f, 1.0f };
    f32 blur_factor = 0.00001f;
    u32 m_GridVAO, m_GridVBO;

    glm::vec2 viewport_size;    
};

}
