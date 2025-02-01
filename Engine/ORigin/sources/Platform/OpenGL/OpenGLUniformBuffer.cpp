// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "OpenGLUniformBuffer.h"
#include "Origin/Core/Assert.h"
#include "Origin/Profiler/Profiler.h"

#include <glad/glad.h>

namespace origin
{
OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t buffer_size, uint32_t binding_point)
		: m_BufferSize(buffer_size), m_BindingPoint(binding_point)
	{
		OGN_PROFILER_RENDERING();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, buffer_size, NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		OGN_PROFILER_RENDERING();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::Bind()
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_RendererID);
	}

	void OpenGLUniformBuffer::Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	uint32_t OpenGLUniformBuffer::Get()
	{
		return m_RendererID;
	}

}
