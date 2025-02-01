// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/EntryPoint.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Core/Input.h"
#include "Origin/GUI/UI.h"
#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/FmodSound.h"
#include "Origin/Audio/FmodDsp.h"
#include "SandboxLayer.h"

#include "backends/imgui_impl_vulkan.h"

#include <ImGuizmo.h>
#include <glad/glad.h>

using namespace origin;

struct TVertex
{
    glm::vec2 position;
    glm::vec3 color;

    static VkVertexInputBindingDescription GetVkBindingDesc()
    {
        VkVertexInputBindingDescription binding_desc = {};
        binding_desc.binding = 0;
        binding_desc.stride = sizeof(TVertex);
        binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding_desc;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetVkAttributeDesc()
    {
        std::array<VkVertexInputAttributeDescription, 2> attr_desc = {};
        // position attribute
        attr_desc[0].binding = 0;
        attr_desc[0].location = 0;
        attr_desc[0].format = VK_FORMAT_R32G32_SFLOAT; // vector 2
        attr_desc[0].offset = offsetof(TVertex, position);

        // color attribute
        attr_desc[1].binding = 0;
        attr_desc[1].location = 1;
        attr_desc[1].format = VK_FORMAT_R32G32B32_SFLOAT; // vector 3
        attr_desc[1].offset = offsetof(TVertex, color);
        return attr_desc;
    }
};

std::vector<TVertex> vertices = {
       {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // Position, Color
       {{-0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
       {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
       {{ 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f}}
};

std::vector<u32> indices = { 0, 1, 2, 2, 3, 0 };

Ref<VulkanGraphicsPipeline> pipeline;
Ref<VulkanBuffer> vertex_buffer;
Ref<VulkanBuffer> index_buffer;

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    vk = VulkanContext::GetInstance();
}

void SandboxLayer::OnAttach()
{
    Physics::Init(PhysicsAPI::Jolt);

    shader = CreateRef<VulkanShader>("Resources/Shaders/Vulkan/default.glsl", true);

    // CREATING VERTEX BUFFER
    VkDeviceSize vertex_buffer_size = sizeof(vertices[0]) * vertices.size();
    vertex_buffer = CreateRef<VulkanBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertex_buffer_size);
    CopyDataToBuffer(vk->GetVkDevice(), vertex_buffer->GetBufferMemory(), vertices.data(), vertex_buffer_size);

    VkDeviceSize indices_buffer_size = sizeof(u32) * indices.size();
    index_buffer = CreateRef<VulkanBuffer>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices_buffer_size);
    CopyDataToBuffer(vk->GetVkDevice(), index_buffer->GetBufferMemory(), indices.data(), indices_buffer_size);

    auto binding_desc = TVertex::GetVkBindingDesc();
    auto attr_desc = TVertex::GetVkAttributeDesc();
    VkPipelineVertexInputStateCreateInfo vertex_info = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    vertex_info.vertexBindingDescriptionCount = 1;
    vertex_info.pVertexBindingDescriptions = &binding_desc;
    vertex_info.vertexAttributeDescriptionCount = static_cast<u32>(attr_desc.size());
    vertex_info.pVertexAttributeDescriptions = attr_desc.data();

    // CREATING PIPELINE
    VkPipelineInputAssemblyStateCreateInfo assm_info = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    assm_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assm_info.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rst_info = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    rst_info.depthClampEnable = VK_FALSE;
    rst_info.rasterizerDiscardEnable = VK_FALSE;
    rst_info.polygonMode = VK_POLYGON_MODE_FILL;
    rst_info.lineWidth = 1.0f;
    rst_info.cullMode = VK_CULL_MODE_FRONT_BIT;
    rst_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rst_info.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo ms_info = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    ms_info.sampleShadingEnable = VK_FALSE;
    ms_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState cb_att = {};
    cb_att.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    cb_att.blendEnable = VK_FALSE;
    VkPipelineColorBlendStateCreateInfo cb_info = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    cb_info.logicOpEnable = VK_FALSE;
    cb_info.logicOp = VK_LOGIC_OP_COPY;
    cb_info.attachmentCount = 1;
    cb_info.pAttachments = &cb_att;
    cb_info.blendConstants[0] = 0.0f;
    cb_info.blendConstants[1] = 0.0f;
    cb_info.blendConstants[2] = 0.0f;
    cb_info.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo layout_info = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    layout_info.setLayoutCount = 0;
    layout_info.pushConstantRangeCount = 0;

    VkViewport viewport = { 
        0.0f, 0.0f,
        static_cast<float>(vk->GetSwapchain()->GetVkExtent2D().width),
        static_cast<float>(vk->GetSwapchain()->GetVkExtent2D().height),
        0.0f, 1.0f
    };
    VkRect2D scissor = { {0, 0}, vk->GetSwapchain()->GetVkExtent2D() };

    pipeline = CreateRef<VulkanGraphicsPipeline>(vk->GetVkDevice());
    pipeline->Create(
        shader->GetShaderStages(),
        vertex_info, 
        assm_info, 
        viewport, 
        scissor,
        vk->GetVkRenderPass(),
        rst_info, 
        ms_info, 
        cb_info,
        layout_info
    );

}

void SandboxLayer::OnDetach()
{
    Physics::Shutdown();

    vertex_buffer->Destroy();
    pipeline->Destroy();
}

void SandboxLayer::OnUpdate(const Timestep delta_time)
{
    vk->Submit([this](VkCommandBuffer cmd, u32 image_index)
    {
        // bind the pipeline
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

        // bind vertex buffer
        VkBuffer buffers[] = { vertex_buffer->GetBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);

        // bind index buffer
        vkCmdBindIndexBuffer(cmd, index_buffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

        // render (6 indices, 1 instance)
        vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);

        // record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    });
}

void SandboxLayer::UpdateCamera(f32 delta_time)
{
    const glm::vec2 &mouse_click_drag_delta = Input::GetMouseClickDragDelta();
}

void SandboxLayer::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<FramebufferResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
    dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseScroll));
}

void SandboxLayer::OnGuiRender()
{
    ImGui::ShowDemoWindow();
}

bool SandboxLayer::OnWindowResize(FramebufferResizeEvent &e)
{
    RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    return false;
}

bool SandboxLayer::OnMouseScroll(MouseScrolledEvent &e)
{
    return false;
}