// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGL_UniformBuffer.h"
#include <glad/glad.h>

namespace Origin
{
	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);

		m_Binding = binding;
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLUniformBuffer::Bind()
	{
		glBindBufferBase(GL_UNIFORM_BUFFER, m_Binding, m_RendererID);
	}

	void OpenGLUniformBuffer::Unbind()
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
		/*
		buffer: The name of the buffer object that you want to update.
		offset: The starting offset within the buffer's data store where the new data will be placed.
		size: The size of the data in bytes that you want to update.
		data: A pointer to the data that you want to update the buffer with.
		*/
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

}
