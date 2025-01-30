// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "VertexArray.h"
#include <glm/glm.hpp>

namespace origin {

class RendererAPI
{
public:
	enum class API
	{
		None = 0,
		OpenGL,
		Vulkan,
		DX11,
		Metal
	};

	virtual void Init() = 0;
	virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
	virtual void ClearColor(const glm::vec4 &color) = 0;
	virtual void ClearColor(float r, float g, float b, float a) = 0;
	virtual void Clear() = 0;
	virtual void DrawIndexed(const std::shared_ptr<VertexArray> &vertexArray, uint32_t IndexCount = 0) = 0;
	virtual void DrawArrays(const std::shared_ptr<VertexArray> &vertexArray, uint32_t count) = 0;
	virtual void DrawLines(const std::shared_ptr<VertexArray> &vertexArray, uint32_t count) = 0;

	virtual void SetLineWidth(float width) = 0;

	static API GetAPI();
	static void SetAPI(API api);
};
}

#endif