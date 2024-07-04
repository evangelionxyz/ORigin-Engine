// Copyright (c) Evangelion Manuhutu | ORigin Engine


#include "OpenGLBuffer.h"
#include "Origin/Profiler/Profiler.h"
#include <glad/glad.h>

#pragma warning(disable : OGN_DISABLED_WARNINGS)

namespace origin
{
	// OpenGL VertexBuffer
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		OGN_PROFILER_RENDERING();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(void *vertices, uint32_t size)
	{
		OGN_PROFILER_RENDERING();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		OGN_PROFILER_RENDERING();

		OpenGLVertexBuffer::Unbind();
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		OGN_PROFILER_RENDERING();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		OGN_PROFILER_RENDERING();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::SetData(const void *data, uint32_t size)
	{
		OGN_PROFILER_RENDERING();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	// OpenGL Index Buffer
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count)
		: m_Count(count)
	{
		OGN_PROFILER_RENDERING();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<uint32_t> indices)
		: m_Count(indices.size() * sizeof(uint32_t))
	{
		OGN_PROFILER_RENDERING();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		OGN_PROFILER_RENDERING();

		OpenGLIndexBuffer::Unbind();
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		OGN_PROFILER_RENDERING();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		OGN_PROFILER_RENDERING();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}