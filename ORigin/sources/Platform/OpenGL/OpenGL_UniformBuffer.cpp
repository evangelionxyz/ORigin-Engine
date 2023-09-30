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

		glNamedBufferData(m_RendererID, bufferSize, nullptr, GL_DYNAMIC_DRAW);
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
		glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint, m_RendererID);
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

	uint32_t OpenGLUniformBuffer::Get()
	{
		return m_RendererID;
	}

}
