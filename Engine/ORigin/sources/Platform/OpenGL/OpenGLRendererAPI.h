// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include "Origin/Renderer/RendererAPI.h"
#include <glm/glm.hpp>

namespace origin {
class OpenGLRendererAPI : public RendererAPI
{
private:
	bool m_DrawLineMode = false;

public:
	virtual void Init() override;
    virtual void SetViewport(u32 x, u32 y, u32 width, u32 height) override;
	virtual void ClearColor(const glm::vec4 &color) override;
	virtual void ClearColor(f32 r, f32 g, f32 b, f32 a) override;
	virtual void Clear() override;
    virtual void DrawIndexed(const Ref<VertexArray> &vertexArray, u32 IndexCount = 0) override;
	virtual void DrawArrays(const Ref<VertexArray> &vertexArray, u32 vertexCount = 0) override;
	virtual void DrawLines(const Ref<VertexArray> &vertexArray, u32 vertexCount = 0) override;
	virtual void SetLineWidth(f32 width) override;
};
}

#endif