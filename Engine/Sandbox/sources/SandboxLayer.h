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

    void RenderShadowMap();

    bool viewport_hovered = false;

    std::vector<f32> CalculateCascadeSplits(f32 near_plane, f32 far_plane, f32 lambda = 0.9f);
    std::array<glm::vec4, 8> TransformToLightSpace(const std::array<glm::vec4, 8> &corners, const glm::mat4 &light_view);
    glm::mat4 CalculateLightProjection(const std::array<glm::vec4, 8> &light_space_corners);
    void UpdateLightSpaceMatrices(const glm::mat4 &light_view, const glm::mat4 &camera_projection, const glm::mat4 &camera_view, const std::vector<f32> &cascade_splits);
    std::array<glm::vec4, 8> GetFrustumCorners(const glm::mat4 &projection, const glm::mat4 &view, f32 near_plane, f32 far_plane);
    glm::mat4 SnapToGrid(const glm::mat4 &light_projection, const glm::mat4 &light_ivew, f32 shadow_map_size);

    Ref<Framebuffer> main_framebuffer;

    EditorCamera camera;

    Ref<FmodSound> roar_sound;

    Ref<Skybox> skybox;
    glm::vec4 tint_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec4 grid_thin_color = { 0.8f, 0.8f , 0.8f, 1.0f };
    glm::vec4 grid_thick_color = { 0.5f, 0.5f, 0.5f, 1.0f };

    f32 blur_factor = 0.00001f;
    Ref<DirectionalLight> dir_light;
    glm::vec3 light_center;

    Ref<Shader> shader;            // base shader
    Ref<Shader> skybox_shader;     // skybox shader
    const i32 NUM_CASCADES = 4;

    std::vector<f32> cascade_splits;
    std::array<glm::mat4, 4> light_space_matrices;

    u32 m_GridVAO, m_GridVBO;

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
