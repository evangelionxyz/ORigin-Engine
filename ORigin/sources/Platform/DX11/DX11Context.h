// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\GraphicsContext.h"
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")

namespace origin
{
	class DX11Context : public GraphicsContext
	{ 
	public:
		DX11Context();
		~DX11Context() override;
		
		void Init(Window *window) override;

		void ResizeSwapChain(uint32_t width, uint32_t height);
		void CreateSwapChain();
		void CleanupDevice();

		static DX11Context *Get();

		ID3D11Device *Device = nullptr;
		IDXGISwapChain *SwapChain = nullptr;
		ID3D11DeviceContext *DeviceContext = nullptr;
		ID3D11RenderTargetView *RenderTargetView = nullptr;
		ID3D11DepthStencilView *DepthStencilView = nullptr;
		D3D11_VIEWPORT Viewport {};
	private:
		
		friend class DX11RendererAPI;
	};
}

