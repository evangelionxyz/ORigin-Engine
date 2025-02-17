// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "Origin/Renderer/Buffer.h"

namespace origin {

	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) = 0;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) = 0;

		virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffer() const = 0;
		virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const = 0;
		static std::shared_ptr<VertexArray> Create();
	};
}

#endif