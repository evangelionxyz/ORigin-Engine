// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "OpenGLBuffer.h"
#include "Origin/Profiler/Profiler.h"
#include <glad/glad.h>

namespace origin
{
	// OpenGL VertexBuffer
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
	{
		OGN_PROFILER_RENDERING();

		// Clear any existing errors
		while (glGetError() != GL_NO_ERROR);

		glCreateBuffers(1, &m_RendererID);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "[glGenBuffers] Error: " << error << std::endl;
			OGN_CORE_ASSERT(false, "[glGenBuffers] {0}", error);
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "[glBindBuffer] Error: " << error << std::endl;
			OGN_CORE_ASSERT(false, "[glBindBuffer] {0}", error);
		}

		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "[glBufferData] Error: " << error << std::endl;
			OGN_CORE_ASSERT(false, "[glBufferData] {0}", error);
		}
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(void *vertices, uint32_t size)
	{
		OGN_PROFILER_RENDERING();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

		GLenum error = glGetError();
		//OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLVertexBuffer.ctor(void*,uint32_t)] {0}", error);
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
		// Clear any existing errors
		while (glGetError() != GL_NO_ERROR);

		OGN_PROFILER_RENDERING();

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		GLenum error = glGetError();
		OGN_CORE_ASSERT(error == GL_NO_ERROR, "[glBindBuffer] {0}", error);

		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
		error = glGetError();
		OGN_CORE_ASSERT(error == GL_NO_ERROR, "[glBufferSubData] {0}", error);
	}

	// OpenGL Index Buffer
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t *indices, uint32_t count)
		: m_Count(count)
	{
        OGN_PROFILER_RENDERING();

        glCreateBuffers(1, &m_RendererID);
        GLenum error = glGetError();
        OGN_CORE_ASSERT(error == GL_NO_ERROR, "OpenGL error after glGenBuffers: {0}", error);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        error = glGetError();
        OGN_CORE_ASSERT(error == GL_NO_ERROR, "OpenGL error after glBindBuffer: {0}", error);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
        error = glGetError();
        OGN_CORE_ASSERT(error == GL_NO_ERROR, "OpenGL error after glBufferData: {0}", error);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(const std::vector<uint32_t> &indices)
		: m_Count(indices.size() * sizeof(uint32_t))
	{
		OGN_PROFILER_RENDERING();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

		GLenum error = glGetError();
		OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLIndexBuffer.ctor(vector<uint32_t>) {0}", error);
	}

    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size)
		: m_Count(size)
    {
		OGN_PROFILER_RENDERING();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

        GLenum error = glGetError();
        OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLIndexBuffer.ctor(uint32_t,uint32_t) {0}", error);
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