// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_VERTEX_ARRAY_H
#define OPENGL_VERTEX_ARRAY_H

#include "Origin/Renderer/VertexArray.h"

namespace origin
{
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray();

		void Bind() const override;
		void Unbind() const override;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) override;
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) override;

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer() const override { return m_VertexBuffer;  }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

	private:
		uint32_t m_RendererID;
		std::vector <std::shared_ptr<VertexBuffer>> m_VertexBuffer;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		uint32_t m_VertexBufferIndex= 0;

	};
};

#endif