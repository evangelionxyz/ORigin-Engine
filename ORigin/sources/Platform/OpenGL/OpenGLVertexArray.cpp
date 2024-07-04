// Copyright (c) Evangelion Manuhutu | ORigin Engine


#include "OpenGLVertexArray.h"
#include "Origin/Core/Assert.h"
#include "Origin/Profiler/Profiler.h"

#pragma warning(disable : OGN_DISABLED_WARNINGS)

namespace origin
{
	static GLenum ShaderDataType_OpenGL(ShaderDataType type)
	{
		OGN_PROFILER_RENDERING();

		switch (type)
		{
		case ShaderDataType::Int:			return GL_INT;
		case ShaderDataType::Int2:		return GL_INT;
		case ShaderDataType::Int3:		return GL_INT;
		case ShaderDataType::Int4:		return GL_INT;

		case ShaderDataType::Float:		return GL_FLOAT;
		case ShaderDataType::Float2:	return GL_FLOAT;
		case ShaderDataType::Float3:	return GL_FLOAT;
		case ShaderDataType::Float4:	return GL_FLOAT;

		case ShaderDataType::Mat2:		return GL_FLOAT;
		case ShaderDataType::Mat3:		return GL_FLOAT;
		case ShaderDataType::Mat4:		return GL_FLOAT;

		case ShaderDataType::Boolean: return GL_BOOL;
		}

		OGN_CORE_ASSERT(false, "Unknown Shader Data Type");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		OGN_PROFILER_RENDERING();

		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		OGN_PROFILER_RENDERING();

		OpenGLVertexArray::Unbind();
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		OGN_PROFILER_RENDERING();

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();
		
		auto layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			switch (element.Type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataType_OpenGL(element.Type),
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					(const void*)element.Offset);
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
			case ShaderDataType::Boolean:
			{
				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribIPointer(m_VertexBufferIndex,
					element.GetComponentCount(),
					ShaderDataType_OpenGL(element.Type),
					layout.GetStride(),
					(const void*)element.Offset);
				m_VertexBufferIndex++;
				break;
			}
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
			{
				uint8_t count = element.GetComponentCount();
				for (uint8_t i = 0; i < count; i++)
				{
					glEnableVertexAttribArray(m_VertexBufferIndex);
					glVertexAttribPointer(m_VertexBufferIndex,
						count,
						ShaderDataType_OpenGL(element.Type),
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(element.Offset + sizeof(float) * count * i));
					glVertexAttribDivisor(m_VertexBufferIndex, 1);
					m_VertexBufferIndex++;
				}
				break;
			}
			default:
				OGN_CORE_ASSERT(false, "Unknown ShaderDataType!");
			}
		}
		m_VertexBuffer.push_back(vertexBuffer);
	}
	
	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		OGN_PROFILER_RENDERING();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

	void OpenGLVertexArray::Bind() const
	{
		OGN_PROFILER_RENDERING();

		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		OGN_PROFILER_RENDERING();

		glBindVertexArray(0);
	}
}