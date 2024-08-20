// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "VulkanRendererAPI.h"
#include "VulkanContext.h"

#include <vulkan.h>
#include <GLFW/glfw3.h>

namespace origin
{
	void VulkanRendererAPI::Init()
	{
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		//GraphicsContext::GetContext<VulkanContext>()->CreateVkSwapchain();
		//OGN_CORE_WARN("Swapchain recreated");
	}

	void VulkanRendererAPI::ClearColor(const glm::vec4 &color)
	{
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