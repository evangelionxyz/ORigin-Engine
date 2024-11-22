// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanRendererAPI.hpp"
#include "VulkanContext.hpp"

namespace origin
{
    void VulkanRendererAPI::Init()
    {
    }

    void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
    }

    void VulkanRendererAPI::ClearColor(const glm::vec4 &color)
    {
        VulkanContext *vk_context = VulkanContext::GetInstance();
        vk_context->m_ClearValue.color.float32[0] = color.r * color.a;
        vk_context->m_ClearValue.color.float32[1] = color.g * color.a;
        vk_context->m_ClearValue.color.float32[2] = color.b * color.a;
        vk_context->m_ClearValue.color.float32[3] = color.a;
    }

    void VulkanRendererAPI::ClearColor(float r, float g, float b, float a)
    {
    }

    void VulkanRendererAPI::Clear()
    {
    }

    void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
    {
    }

    void VulkanRendererAPI::DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)
    {
    }

    void VulkanRendererAPI::DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)
    {
    }

    void VulkanRendererAPI::SetLineWidth(float width)
    {
    }
}