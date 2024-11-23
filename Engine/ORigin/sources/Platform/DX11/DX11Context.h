// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef DX11_CONTEXT_H
#define DX11_CONTEXT_H

#include "Origin/Renderer/GraphicsContext.h"
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
		void Shutdown() override;

		void ResizeSwapChain(uint32_t width, uint32_t height);
		void CreateSwapChain();
		void CleanupDevice();

		static DX11Context *GetInstance();

		ID3D11Device *GetDevice() const { return m_Device; }
		IDXGISwapChain *GetSwapchain() const { return m_Swapchain;  }
		ID3D11DeviceContext *GetDeviceContext() const { return m_DeviceContext; }

		
	private:
        ID3D11Device *m_Device = nullptr;
        IDXGISwapChain *m_Swapchain = nullptr;
        ID3D11DeviceContext *m_DeviceContext = nullptr;
        ID3D11RenderTargetView *m_RenderTargetView = nullptr;
        ID3D11DepthStencilView *m_DepthStencilView = nullptr;

        D3D11_VIEWPORT m_viewport{};

		static DX11Context *s_Instance;

		friend class DX11RendererAPI;
	};
}

#endif