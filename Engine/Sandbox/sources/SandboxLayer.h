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
    void RenderDebug(f32 delta_time);
    void InitSounds();

    void InitCascadeShadow();
    void InitDebugRenderer();
    void CleanupFramebuffers();

    void RenderShadowMap();

    bool viewport_hovered = false;

    std::vector<f32> CalculateCascadeSplits(f32 near_plane, f32 far_plane);
    void UpdateLightSpaceMatrices(const glm::vec3 &light_dir, const glm::mat4 &view_matrix, f32 fov, f32 aspect, f32 near_plane);
    std::vector<glm::vec4> GetFrustumCorners(f32 fov, f32 aspect, f32 near_plane, f32 far_plane);

    Ref<Framebuffer> main_framebuffer;

    EditorCamera camera;

    Ref<FmodSound> roar_sound;

    Ref<Skybox> skybox;
    glm::vec4 tint_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    f32 blur_factor = 0.00001f;
    Ref<DirectionalLight> dir_light;
    glm::vec3 light_center;

    Ref<Shader> shader;            // base shader
    Ref<Shader> skybox_shader;     // skybox shader
    const i32 NUM_CASCADES = 4;

    std::vector<f32> cascade_splits;
    std::vector<glm::mat4> light_space_matrices;

    glm::vec2 viewport_size;
    
    struct CascadeShadow
    {
        Ref<Shader> shader;
        Ref<Framebuffer> framebuffer;
        std::vector<f32> shadow_bias;
    } csm;

    struct ShadowMapDeug
    {
        u32 vao;
        Ref<Shader> shader;
        Ref<Framebuffer> framebuffer;
    } debug_r;

};

}
