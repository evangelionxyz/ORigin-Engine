// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "OpenGLBuffer.h"
#include "Origin/Profiler/Profiler.h"

#include <glad/glad.h>

namespace origin {
// OpenGL VertexBuffer
OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
{
	OGN_PROFILER_RENDERING();

	// Clear any existing errors
	while (glGetError() != GL_NO_ERROR);

	glCreateBuffers(1, &m_BufferID);
	GLenum error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLVertexBuffer] glCreateBuffers GL_ARRAY_BUFFER {}", error);

	glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLVertexBuffer] glBindBuffer GL_ARRAY_BUFFER {}", error);

	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLVertexBuffer] glBindBuffer glBufferData GL_ARRAY_BUFFER {}", error);
}

OpenGLVertexBuffer::OpenGLVertexBuffer(void *vertices, uint32_t size)
{
	OGN_PROFILER_RENDERING();

	// Clear any existing errors
	while (glGetError() != GL_NO_ERROR);

	glCreateBuffers(1, &m_BufferID);
	GLenum error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLVertexBuffer] glCreateBuffers GL_ARRAY_BUFFER {}", error);

	glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLVertexBuffer] glBindBuffer GL_ARRAY_BUFFER {}", error);

	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLVertexBuffer] glBindBuffer glBufferData GL_ARRAY_BUFFER {}", error);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
	glDeleteBuffers(1, &m_BufferID);
}

void OpenGLVertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
}

void OpenGLVertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVertexBuffer::SetData(const void *data, uint32_t size, uint32_t offset)
{
	OGN_PROFILER_RENDERING();

	while (glGetError() != GL_NO_ERROR);

	glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	GLenum error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLVertexBuffer] glBufferSubData GL_ARRAY_BUFFER {}", error);
}

// ======================
// OpenGL Index Buffer

OpenGLIndexBuffer::OpenGLIndexBuffer(void *indices, uint32_t count)
	: m_Count(count)
{
	OGN_PROFILER_RENDERING();

	glCreateBuffers(1, &m_BufferID);
	GLenum error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLIndexBuffer] glCreateBuffer GL_ELEMENT_ARRAY_BUFFER {}", error);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLIndexBuffer] glBindBuffer GL_ELEMENT_ARRAY_BUFFER {}", error);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLIndexBuffer] glBufferData GL_ELEMENT_ARRAY_BUFFER {}", error);
}

OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t size)
	: m_Count(0)
{
	OGN_PROFILER_RENDERING();

	glCreateBuffers(1, &m_BufferID);
	GLenum error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLIndexBuffer] glCreateBuffer GL_ELEMENT_ARRAY_BUFFER {}", error);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLIndexBuffer] glBindBuffer GL_ELEMENT_ARRAY_BUFFER {}", error);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	error = glGetError();
	OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLIndexBuffer] glBufferData GL_ELEMENT_ARRAY_BUFFER {}", error);
}

OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
	glDeleteBuffers(1, &m_BufferID);
}

void OpenGLIndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
}

void OpenGLIndexBuffer::Unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}