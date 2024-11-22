// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef VULKAN_RENDERER_API_H
#define VULKAN_RENDERER_API_H

#include "Origin/Renderer/RendererAPI.h"

namespace origin
{
    class VulkanRendererAPI : public RendererAPI
	{
	private:
		bool m_DrawLineMode = false;

	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void ClearColor(const glm::vec4 &color) override;
		virtual void ClearColor(float r, float g, float b, float a) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray> &vertexArray, uint32_t IndexCount = 0) override;
		virtual void DrawArrays(const std::shared_ptr<VertexArray> &vertexArray, uint32_t vertexCount = 0) override;
		virtual void DrawLines(const std::shared_ptr<VertexArray> &vertexArray, uint32_t vertexCount = 0) override;
		virtual void SetLineWidth(float width) override;
	};
}

#endif