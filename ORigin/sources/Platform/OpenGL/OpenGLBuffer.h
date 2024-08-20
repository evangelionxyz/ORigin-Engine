// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_BUFFER_H
#define OPENGL_BUFFER_H

#include "Origin/Renderer/Buffer.h"

namespace origin
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(void* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();
		
		void Bind() const override;
		void Unbind() const override;
		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	private:
		uint32_t m_BufferID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t size);
		OpenGLIndexBuffer(void* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		void Bind() const override;
		void Unbind() const override;
		uint32_t GetCount() const override { return m_Count; }
		uint32_t GetBufferID() const override { return m_BufferID; } 

	private:
		uint32_t m_BufferID;
		uint32_t m_Count = 0;
	};
}

#endif