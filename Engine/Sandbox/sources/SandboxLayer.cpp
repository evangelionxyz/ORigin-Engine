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
};

Ref<Texture2D> image;
Ref<VulkanGraphicsPipeline> pipeline;
Ref<VulkanBuffer> vertex_buffer;
Ref<VulkanBuffer> index_buffer;
Ref<VulkanBuffer> uniform_buffer_obj;
VkDescriptorSet descriptor_set;
VkDescriptorSetLayout descriptor_set_layout;
glm::vec3 obj_position{ 0.0f, 0.0f, -1.0f };

void UpdateDescriptorSet(VkDescriptorSet decriptor_set, VkBuffer uniform_buffer, VkImageView texture_image_view, VkSampler texture_sampler)
{
    // Descriptor for Uniform Buffer (binding 0)
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = uniform_buffer;
    buffer_info.offset = 0;
    buffer_info.range = sizeof(UniformBufferObject);

    // Descriptor for image sampler (binding 1)
    VkDescriptorImageInfo image_info{};
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = texture_image_view;
    image_info.sampler = texture_sampler;

    // descriptor write array
    std::array< VkWriteDescriptorSet, 2> descriptor_writes{};

    // uniform buffer binding
    descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[0].dstSet = decriptor_set;
    descriptor_writes[0].dstArrayElement = 0;
    descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptor_writes[0].descriptorCount = 1;
    descriptor_writes[0].pBufferInfo = &buffer_info;

    // image sampler binding
    descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_writes[1].dstSet = decriptor_set;
    descriptor_writes[1].dstBinding = 1;  // Matches the binding in the shader
    descriptor_writes[1].dstArrayElement = 0;
    descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptor_writes[1].descriptorCount = 1;
    descriptor_writes[1].pImageInfo = &image_info;

    vkUpdateDescriptorSets(VulkanContext::GetInstance()->GetVkDevice(),
        static_cast<u32>(descriptor_writes.size()), 
        descriptor_writes.data(), 
        0, nullptr);
}

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
    shader = CreateRef<VulkanShader>("Resources/Shaders/Vulkan/default.glsl", true);

    // create descriptor set
    std::array<VkDescriptorSetLayoutBinding, 2> bindings{};
    // Uniform buffer binding
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    // Image sampler binding
    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo descriptor_layout_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
    descriptor_layout_info.bindingCount = static_cast<u32>(bindings.size());
    descriptor_layout_info.pBindings = bindings.data();

    VkResult result = vkCreateDescriptorSetLayout(vk->GetVkDevice(),
        &descriptor_layout_info,
        nullptr,
        &descriptor_set_layout);

    VK_ERROR_CHECK(result, "[Vulkan] Failed to create descriptor set layout");

    VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
    alloc_info.descriptorPool = vk->GetVkDescriptorPool();
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &descriptor_set_layout;

    result = vkAllocateDescriptorSets(vk->GetVkDevice(), &alloc_info, &descriptor_set);
    VK_ERROR_CHECK(result, "[Vulkan] Failed to create descriptor");

    VkDeviceSize uniform_buffer_size = sizeof(UniformBufferObject);
    uniform_buffer_obj = CreateRef<VulkanBuffer>(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, uniform_buffer_size);
    UpdateDescriptorSet(descriptor_set, 
        uniform_buffer_obj->GetBuffer(), 
        ((VulkanImage *)image.get())->GetImageView(), 
        ((VulkanImage *)image.get())->GetSampler());

    // CREATING VERTEX BUFFER
    VkDeviceSize vertex_buffer_size = sizeof(vertices[0]) * vertices.size();
    vertex_buffer = CreateRef<VulkanBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertex_buffer_size);
    vertex_buffer->SetData(vertices.data(), vertex_buffer_size);

    VkDeviceSize indices_buffer_size = sizeof(u32) * indices.size();
    index_buffer = CreateRef<VulkanBuffer>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices_buffer_size);
    index_buffer->SetData(indices.data(), indices_buffer_size);

    auto binding_desc = TVertex::GetVkBindingDesc();
    auto attr_desc = TVertex::GetVkAttributeDesc();
    VkPipelineVertexInputStateCreateInfo vertex_input_state = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    vertex_input_state.vertexBindingDescriptionCount = 1;
    vertex_input_state.pVertexBindingDescriptions = &binding_desc;
    vertex_input_state.vertexAttributeDescriptionCount = static_cast<u32>(attr_desc.size());
    vertex_input_state.pVertexAttributeDescriptions = attr_desc.data();

    // CREATING PIPELINE
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
    pipeline_layout_ci.pSetLayouts = &descriptor_set_layout;

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

    // Submit to Vulkan command buffer
    vk->Submit([this](VkCommandBuffer cmd, VkFramebuffer framebuffer, u32 image_index)
    {
        // bind the pipeline
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetPipeline());

        // bind descriptor sets
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetLayout(),
            0, 1, &descriptor_set, 0, nullptr);

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
    uniform_buffer_obj->Destroy();

    vkDestroyDescriptorSetLayout(vk->GetVkDevice(), descriptor_set_layout, nullptr);

    pipeline->Destroy();
    image->Destroy();

}

void SandboxLayer::OnUpdate(const Timestep delta_time)
{
    UpdateCamera(delta_time);

    UniformBufferObject ubo{};
    ubo.view_projection = camera.GetViewProjection();
    ubo.model_transform = glm::translate(glm::mat4(1.0f), obj_position);

    void *data;
    vkMapMemory(vk->GetVkDevice(), uniform_buffer_obj->GetMemory(), 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(vk->GetVkDevice(), uniform_buffer_obj->GetMemory());
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