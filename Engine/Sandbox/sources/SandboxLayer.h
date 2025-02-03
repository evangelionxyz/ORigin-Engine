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
    void CreatePipeline();

    void UpdateCamera(f32 delta_time);

    EditorCamera camera;
    VulkanContext *vk = nullptr;
    Ref<VulkanShader> shader;
    Ref<Texture2D> image;
    Ref<VulkanBuffer> vertex_buffer;
    Ref<VulkanBuffer> index_buffer;
    Ref<VulkanBuffer> uniform_buffer;
    Ref<VulkanGraphicsPipeline> pipeline;
    Ref<VulkanDescriptorSet> descriptor_set;


};

}
