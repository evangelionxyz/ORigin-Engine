#include "pch.h"
#include "DX11Context.h"
#include "DX11RendererAPI.h"

#include <glm/gtc/type_ptr.hpp>

namespace origin
{

	void DX11RendererAPI::Init()
	{
	}

	void DX11RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		DX11Context::Get()->ResizeSwapChain(width, height);
	}

	void DX11RendererAPI::ClearColor(const glm::vec4 &color)
	{
		DX11Context::Get()->DeviceContext->ClearRenderTargetView(DX11Context::Get()->RenderTargetView, glm::value_ptr(color));
	}

	void DX11RendererAPI::ClearColor(float r, float g, float b, float a)
	{
		DX11RendererAPI::ClearColor(glm::vec4(r, g, b, a));
	}

	void DX11RendererAPI::Clear()
	{
		//DX11Context::Get()->DeviceContext->OMSetRenderTargets(1, &DX11Context::Get()->RenderTargetView, nullptr);
	}

	void DX11RendererAPI::DrawIndexed(const std::shared_ptr<VertexArray> &vertexArray, uint32_t IndexCount)
	{
	}

	void DX11RendererAPI::DrawArrays(const std::shared_ptr<VertexArray> &vertexArray, uint32_t vertexCount)
	{

	}

	void DX11RendererAPI::DrawLines(const std::shared_ptr<VertexArray> &vertexArray, uint32_t vertexCount)
	{

	}

	void DX11RendererAPI::SetLineWidth(float width)
	{

	}

}