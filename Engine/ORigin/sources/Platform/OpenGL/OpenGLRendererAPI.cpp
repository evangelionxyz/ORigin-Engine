// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace origin {
void OpenGLRendererAPI::Init()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLRendererAPI::SetViewport(u32 x, u32 y, u32 width, u32 height)
{
	glViewport(x, y, width, height);
}

void OpenGLRendererAPI::ClearColor(const glm::vec4 &color)
{
	glClearColor(color.r * color.a, color.g * color.a, color.b * color.a, color.a);
}

void OpenGLRendererAPI::ClearColor(f32 r, f32 g, f32 b, f32 a)
{
	glClearColor(r * a, g * a, b * a, a);
}

void OpenGLRendererAPI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray> &vertexArray, u32 indexCount)
{
	if (vertexArray)
	{
		vertexArray->Bind();
		u32 count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();

		// Ensure that the index buffer is correctly bound
		OGN_CORE_ASSERT(count > 0, "Index count is zero");
		// GLenum error = glGetError();
        // OGN_CORE_ASSERT(error == GL_NO_ERROR, "[GL ERROR] {}", error);

		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		// error = glGetError();
		// OGN_CORE_ASSERT(error == GL_NO_ERROR, "[GL ERROR] {}", error);
	}
}

void OpenGLRendererAPI::DrawArrays(const Ref<VertexArray> &vertexArray, u32 vertexCount)
{
	m_DrawLineMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (vertexArray)
	{
		vertexArray->Bind();
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
}

void OpenGLRendererAPI::DrawLines(const Ref<VertexArray> &vertexArray, u32 vertexCount)
{
	if (vertexArray)
	{
		vertexArray->Bind();
		glDrawArrays(GL_LINES, 0, vertexCount);
	}
}

void OpenGLRendererAPI::SetLineWidth(f32 width)
{
	glLineWidth(width);
}
}