// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "RendererAPI.h"

namespace origin {

	class RenderCommand
	{
	public:

		static void Init()
		{
			s_RendererAPI->Init();
		}

		static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void ClearColor(const glm::vec4& color)
		{
			s_RendererAPI->ClearColor(color);
		}

		static void ClearColor(float r, float g, float b, float a)
		{
			s_RendererAPI->ClearColor(r, g, b, a);
		}

		static void Clear()
		{
			s_RendererAPI->Clear();
		}

		static void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, indexCount);
		}

		static void DrawArrays(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_RendererAPI->DrawArrays(vertexArray, vertexCount);
		}

		static void DrawLines(const std::shared_ptr<VertexArray>& vertexArray, uint32_t vertexCount)
		{
			s_RendererAPI->DrawLines(vertexArray, vertexCount);
		}

		static void SetLineWidth(float width)
		{
			s_RendererAPI->SetLineWidth(width);
		}

		static RendererAPI* s_RendererAPI;
	};

}

