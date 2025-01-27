#pragma once
#include <Origin.hpp>

#include <random>
#include <chrono>

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
    bool OnKeyPressed(KeyPressedEvent &e);
    bool OnMouseButtonPressed(MouseButtonPressedEvent &e);
    bool OnMouseMove(MouseMovedEvent &e);
    bool OnMouseScroll(MouseScrolledEvent &e);

private:
    void UpdateCamera(f32 delta_time);
    void UpdateCharacterMovement(f32 delta_time);
    void RenderScene(f32 delta_time);
    void RenderSkybox(f32 delta_time);
    void RenderDebug(f32 delta_time);

    void InitSounds();

    EditorCamera camera;
    EditorCamera camera2;

    Ref<FmodSound> roar_sound;

    Ref<Skybox> skybox;
    glm::vec4 tint_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    f32 blur_factor = 0.00001f;
    Ref<DirectionalLight> dir_light;

    Ref<Shader> shader;            // base shader
    Ref<Shader> skybox_shader;     // skybox shader
    Ref<Shader> shadow_map_shader; // shadow map shader
    u32 shadow_map_fbo;
    u32 shadow_map_texture;
    const u32 shadow_map_resolution = 640;

};

}
