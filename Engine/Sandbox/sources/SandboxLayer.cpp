// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "Origin/Asset/AssetImporter.h"
#include "Origin/Audio/AudioEngine.h"
#include "Origin/Audio/FmodDsp.h"
#include "Origin/Audio/FmodSound.h"
#include "Origin/Core/Input.h"
#include "Origin/EntryPoint.h"
#include "Origin/GUI/UI.h"
#include "SandboxLayer.h"

#include "backends/imgui_impl_vulkan.h"

#include <glad/glad.h>
#include <ImGuizmo.h>

using namespace origin;

struct TVertex
{
    glm::vec2 position;
    glm::vec2 texcoord;

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
        attr_desc[0].binding = 0;
        attr_desc[0].location = 0;
        attr_desc[0].format = VK_FORMAT_R32G32_SFLOAT; // vector 2
        attr_desc[0].offset = offsetof(TVertex, position);

        attr_desc[1].binding = 0;
        attr_desc[1].location = 1;
        attr_desc[1].format = VK_FORMAT_R32G32_SFLOAT; // vector 2
        attr_desc[1].offset = offsetof(TVertex, texcoord);
        return attr_desc;
    }
};

std::vector<TVertex> vertices = {
       {{-0.5f, -0.5f}, {0.0f, 0.0f}}, // Position, Color
       {{-0.5f,  0.5f}, {0.0f, 1.0f}},
       {{ 0.5f,  0.5f}, {1.0f, 1.0f}},
       {{ 0.5f, -0.5f}, {1.0f, 0.0f}}
};

std::vector<u32> indices = { 0, 1, 2, 2, 3, 0 };


struct UniformBufferObject
{
    glm::mat4 view_projection;
    glm::mat4 model_transform;
    glm::vec3 camera_position;
};

glm::vec3 obj_position{ 0.0f, 0.0f, -1.0f };

SandboxLayer::SandboxLayer() : Layer("Sandbox")
{
    camera.InitPerspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    camera.SetPosition({ 0.0f, 0.0f, 0.0f });

    vk = VulkanContext::GetInstance();
}

void SandboxLayer::OnAttach()
{
    Physics::Init(PhysicsAPI::Jolt);

    image = Texture2D::Create("Resources/icon.png");
    shader = CreateRef<VulkanShader>("Resources/Shaders/Vulkan/default.glsl", false);

    VulkanDescriptorSetLayout layout = {
        {"position", VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0 },
        {"image_sampler", VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1 }
    };
    descriptor_set = CreateRef<VulkanDescriptorSet>(layout);

    uniform_buffer = CreateRef<VulkanBuffer>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UniformBufferObject));

    // Descriptor for Uniform Buffer (binding 0)
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = uniform_buffer->GetBuffer();
    buffer_info.offset = 0;
    buffer_info.range = sizeof(UniformBufferObject);

    // uniform buffer binding
    VkWriteDescriptorSet write_a{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write_a.dstSet = *descriptor_set->GetDescriptorSet();
    write_a.dstArrayElement = 0;
    write_a.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_a.descriptorCount = 1;
    write_a.pBufferInfo = &buffer_info;

    // Descriptor for image sampler (binding 1)
    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = ((VulkanImage*)image.get())->GetImageView();
    image_info.sampler = ((VulkanImage*)image.get())->GetSampler();

    // image sampler binding
    VkWriteDescriptorSet write_b{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
    write_b.dstSet = *descriptor_set->GetDescriptorSet();
    write_b.dstBinding = 1;
    write_b.dstArrayElement = 0;
    write_b.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write_b.descriptorCount = 1;
    write_b.pImageInfo = &image_info;

    descriptor_set->Update({write_a, write_b});
   
    CreatePipeline();

    // Submit to Vulkan command buffer
    vk->Submit([this](VkCommandBuffer cmd, VkFramebuffer framebuffer, u32 image_index)
    {
        // bind the pipeline
        pipeline->Bind(cmd);

        // bind descriptor sets
        descriptor_set->Bind(cmd, pipeline->GetLayout());

        // bind vertex buffer
        VkBuffer buffers[] = { vertex_buffer->GetBuffer() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmd, 0, 1, buffers, offsets);

        // bind index buffer
        vkCmdBindIndexBuffer(cmd, index_buffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

        // render (6 indices, 1 instance)
        vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);
    });
}

void SandboxLayer::OnDetach()
{
    Physics::Shutdown();

    vertex_buffer->Destroy();
    index_buffer->Destroy();
    uniform_buffer->Destroy();

    descriptor_set->Destroy();

    pipeline->Destroy();
    image->Destroy();
}

void SandboxLayer::OnUpdate(const Timestep delta_time)
{
    UpdateCamera(delta_time);

    UniformBufferObject ubo{};
    ubo.view_projection = camera.GetViewProjection();
    ubo.model_transform = glm::translate(glm::mat4(1.0f), obj_position);
    ubo.camera_position = camera.GetPosition();

    uniform_buffer->SetData(&ubo, sizeof(ubo));
}

void SandboxLayer::CreatePipeline()
{
    // CREATING PIPELINE
    VkDeviceSize vertex_buffer_size = sizeof(vertices[0]) * vertices.size();
    vertex_buffer = CreateRef<VulkanBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertex_buffer_size);
    vertex_buffer->SetData(vertices.data(), vertex_buffer_size);

    VkDeviceSize indices_buffer_size = sizeof(u32) * indices.size();
    index_buffer = CreateRef<VulkanBuffer>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices_buffer_size);
    index_buffer->SetData(indices.data(), indices_buffer_size);

    VkVertexInputBindingDescription binding_desc = TVertex::GetVkBindingDesc();
    auto attr_desc = TVertex::GetVkAttributeDesc();
    VkPipelineVertexInputStateCreateInfo vertex_input_state = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    vertex_input_state.vertexBindingDescriptionCount = 1;
    vertex_input_state.pVertexBindingDescriptions = &binding_desc;
    vertex_input_state.vertexAttributeDescriptionCount = static_cast<u32>(attr_desc.size());
    vertex_input_state.pVertexAttributeDescriptions = attr_desc.data();

    VkPipelineInputAssemblyStateCreateInfo input_assembly_state = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_state.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterization_state = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    rasterization_state.depthClampEnable = VK_FALSE;
    rasterization_state.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_state.lineWidth = 1.0f;
    rasterization_state.cullMode = VK_CULL_MODE_FRONT_BIT;
    rasterization_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterization_state.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisample_state = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisample_state.sampleShadingEnable = VK_FALSE;
    multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState color_blend_attachment = {};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = VK_FALSE;
    VkPipelineColorBlendStateCreateInfo color_blend_state = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    color_blend_state.logicOpEnable = VK_FALSE;
    color_blend_state.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state.attachmentCount = 1;
    color_blend_state.pAttachments = &color_blend_attachment;
    color_blend_state.blendConstants[0] = 0.0f;
    color_blend_state.blendConstants[1] = 0.0f;
    color_blend_state.blendConstants[2] = 0.0f;
    color_blend_state.blendConstants[3] = 0.0f;

    VkPipelineLayoutCreateInfo pipeline_layout_ci = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
    pipeline_layout_ci.setLayoutCount = 1;
    pipeline_layout_ci.pSetLayouts = descriptor_set->GetLayout();

    VkViewport viewport = { 0.0f, 0.0f,
        static_cast<float>(vk->GetSwapchain()->GetVkExtent2D().width),
        static_cast<float>(vk->GetSwapchain()->GetVkExtent2D().height),
        0.0f, 1.0f
    };

    VkRect2D scissor = { {0, 0}, vk->GetSwapchain()->GetVkExtent2D() };

    PipelineCreateInfo pipeline_ci{};
    pipeline_ci.render_pass = vk->GetVkRenderPass();
    pipeline_ci.shader_stages = shader->GetStages();
    pipeline_ci.vertex_input_state = vertex_input_state;
    pipeline_ci.input_assembly_state = input_assembly_state;
    pipeline_ci.rasterization_state = rasterization_state;
    pipeline_ci.multisample_state = multisample_state;
    pipeline_ci.layout = pipeline_layout_ci;
    pipeline_ci.viewport = viewport;
    pipeline_ci.scissor = scissor;
    pipeline_ci.color_blend_state = color_blend_state;

    pipeline = CreateRef<VulkanGraphicsPipeline>(pipeline_ci);
}

void SandboxLayer::UpdateCamera(f32 delta_time)
{
    const glm::vec2 &mouse_click_drag_delta = Input::GetMouseClickDragDelta();
    camera.OnMouseMove({ mouse_click_drag_delta.x, -mouse_click_drag_delta.y });
    camera.OnUpdate(delta_time);
    camera.UpdateView();
    camera.UpdateProjection();
}

void SandboxLayer::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<FramebufferResizeEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnWindowResize));
    dispatcher.Dispatch<MouseScrolledEvent>(OGN_BIND_EVENT_FN(SandboxLayer::OnMouseScroll));

    camera.OnEvent(e);
}

void SandboxLayer::OnGuiRender()
{
    ImGui::ShowDemoWindow();
}

bool SandboxLayer::OnWindowResize(FramebufferResizeEvent &e)
{
    RenderCommand::SetViewport(0, 0, e.GetWidth(), e.GetHeight());
    camera.SetViewportSize(e.GetWidth(), e.GetHeight());
    return false;
}

bool SandboxLayer::OnMouseScroll(MouseScrolledEvent &e)
{
    return false;
}