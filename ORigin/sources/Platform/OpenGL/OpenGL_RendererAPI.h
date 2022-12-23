// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\RendererAPI.h"
#include <glm\glm.hpp>

namespace Origin
{
	class OpenGLRendererAPI : public RendererAPI
	{
	private:
		bool m_DrawLineMode = false;

	public:
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void ClearColor(glm::vec4 color) override;
		virtual void ClearColor(float r, float g, float b, float a) override;

		virtual void Clear() override;

		virtual void DrawLineMode(bool enable = false) override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t IndexCount = 0) override;
		virtual void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount = 0) override;
		virtual void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount = 0) override;
		virtual void SetLineWidth(float width) override;
	};
}