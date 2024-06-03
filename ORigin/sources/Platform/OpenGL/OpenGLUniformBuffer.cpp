// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGLUniformBuffer.h"
#include <glad/glad.h>

namespace origin
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t bufferSize, uint32_t bindingPoint)
		: m_BindingPoint(bindingPoint)
	{
		OGN_PROFILER_RENDERING();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		OGN_PROFILER_RENDERING();

		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::Bind()
	{
		OGN_PROFILER_RENDERING();

		glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_RendererID);
	}

	void OpenGLUniformBuffer::Unbind()
	{
		OGN_PROFILER_RENDERING();

		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		OGN_PROFILER_RENDERING();

		// call this had any changes
		
		// try to use glBindBufferBase (Bind function) also
		// if the uniform does not retrieve any changes
		
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	}

	uint32_t OpenGLUniformBuffer::Get()
	{
		return m_RendererID;
	}

}
