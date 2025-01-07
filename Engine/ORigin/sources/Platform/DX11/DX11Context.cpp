// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "DX11Context.h"

#include <Origin/Core/Window.h>

#include <GLFW/glfw3native.h>

namespace origin
{
	DX11Context *DX11Context::s_Instance = nullptr;
	
	DX11Context::DX11Context()
	{
		s_Instance = this;
	}

	DX11Context::~DX11Context()
	{
		CleanupDevice();
	}

	void DX11Context::Init(Window *window)
	{
		OGN_PROFILER_RENDERING();

		DXGI_SWAP_CHAIN_DESC sc;
		ZeroMemory(&sc, sizeof(DXGI_SWAP_CHAIN_DESC));
		
		sc.BufferCount = 1; // Double buffered
		sc.BufferDesc.Width = 0;
		sc.BufferDesc.Height = 0;
		sc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sc.BufferDesc.RefreshRate.Numerator = 0;
		sc.BufferDesc.RefreshRate.Denominator = 0;
		sc.SampleDesc.Count = 1;
		sc.SampleDesc.Quality = 0;
		sc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sc.OutputWindow = glfwGetWin32Window((GLFWwindow *)window->GetNativeWindow());
		sc.Windowed = TRUE;

		constexpr D3D_FEATURE_LEVEL featureLevels[] = 
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0, 
			D3D_FEATURE_LEVEL_10_0, 
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_1
		};

		D3D_FEATURE_LEVEL featureLevel;
		UINT createDeviceFlags = 0;

#ifdef OGN_DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT result = D3D11CreateDeviceAndSwapChain(
			   nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			   createDeviceFlags, featureLevels, ARRAYSIZE(featureLevels),
			   D3D11_SDK_VERSION, &sc, &m_Swapchain, &m_Device,
			   &featureLevel, &m_DeviceContext
		   );

		OGN_CORE_ASSERT(FAILED(result) == 0, "[DX11 Context] Failed to create Device and Swap Chain");
		memset(&m_viewport, 0, sizeof(D3D11_VIEWPORT));
		m_viewport.Width= static_cast<FLOAT>(window->GetWidth());
		m_viewport.Height = static_cast<FLOAT>(window->GetHeight());
		CreateSwapChain();

		OGN_CORE_TRACE("DirectX 11 Info");
		OGN_CORE_TRACE(" SDK Version : {}", D3D11_SDK_VERSION);
		//OGN_CORE_TRACE(" Vendor      : {}", D3D11_VENDOR);
		//OGN_CORE_TRACE(" Renderer    : {}", glGetString(GL_RENDERER));
	}

	void DX11Context::Shutdown()
	{
		m_Device->Release();
	}

	DX11Context* DX11Context::GetInstance()
	{
		return s_Instance;
	}

	void DX11Context::CreateSwapChain()
	{
		OGN_PROFILER_RENDERING();

		ID3D11Texture2D *backBuffer = nullptr;
		m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));

		m_Device->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);
		backBuffer->Release();
		
		// Bind 1 view to RenderTargetView
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);
		
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = m_viewport.TopLeftY = 0.0f;
		m_DeviceContext->RSSetViewports(1, &m_viewport);
	}

	void DX11Context::ResizeSwapChain(uint32_t width, uint32_t height)
	{
		OGN_PROFILER_RENDERING();

		if (m_DeviceContext) m_DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

		// Release all outstanding references to the swap chain's buffers.
		if (m_RenderTargetView) m_RenderTargetView->Release();

		// Preserve the existing buffer count and format
		m_Swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

		ID3D11Texture2D *backBuffer = nullptr;
		m_Swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));
		m_Device->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView);
		backBuffer->Release();

		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);

		memset(&m_viewport, 0, sizeof(D3D11_VIEWPORT));
		m_viewport.Width = static_cast<FLOAT>(width);
		m_viewport.Height = static_cast<FLOAT>(height);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = m_viewport.TopLeftY = 0.0f;
		m_DeviceContext->RSSetViewports(1, &m_viewport);
	}

	void DX11Context::CleanupDevice()
	{
		OGN_PROFILER_RENDERING();

		if (m_RenderTargetView)
		{
			m_RenderTargetView->Release();
			m_RenderTargetView = nullptr;
		}

		if (m_Swapchain)
		{
			m_Swapchain->Release();
			m_Swapchain = nullptr;
		}

		if (m_Device)
		{
			m_Device->Release();
			m_Device = nullptr;
		}

		if (m_DeviceContext)
		{
			m_DeviceContext->Release();
			m_DeviceContext = nullptr;
		}
	}
}
