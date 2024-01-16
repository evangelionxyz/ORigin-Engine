// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGL_UniformBuffer.h"
#include <glad/glad.h>

namespace origin
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t bufferSize, uint32_t bindingPoint)
		: m_BindingPoint(bindingPoint)
	{
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, bufferSize, NULL, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
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
