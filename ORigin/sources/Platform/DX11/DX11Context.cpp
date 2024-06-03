#include "pch.h"
#include "DX11Context.h"

#ifdef OGN_WINDOWS_PLATFORM
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW\glfw3native.h"
#endif

namespace origin
{
	static DX11Context *s_Instance = nullptr;
	
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
		sc.OutputWindow = glfwGetWin32Window(window->GetNativeWindow());
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
			   D3D11_SDK_VERSION, &sc, &SwapChain, &Device,
			   &featureLevel, &DeviceContext
		   );

		OGN_CORE_ASSERT(FAILED(result) == 0, "[DX11 Context] Failed to create Device and Swap Chain");
		memset(&Viewport, 0, sizeof(D3D11_VIEWPORT));
		Viewport.Width= static_cast<FLOAT>(window->GetWidth());
		Viewport.Height = static_cast<FLOAT>(window->GetHeight());
		CreateSwapChain();

		OGN_CORE_TRACE("DirectX 11 Info");
		OGN_CORE_TRACE(" SDK Version : {0}", D3D11_SDK_VERSION);
		//OGN_CORE_TRACE(" Vendor      : {0}", D3D11_VENDOR);
		//OGN_CORE_TRACE(" Renderer    : {0}", glGetString(GL_RENDERER));
	}

	DX11Context* DX11Context::Get()
	{
		return s_Instance;
	}

	void DX11Context::CreateSwapChain()
	{
		OGN_PROFILER_RENDERING();

		ID3D11Texture2D *backBuffer = NULL;
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));
		Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);
		backBuffer->Release();
		
		// Bind 1 view to RenderTargetView
		DeviceContext->OMSetRenderTargets(1, &RenderTargetView, NULL);
		
		Viewport.MinDepth = 0.0f;
		Viewport.MaxDepth = 1.0f;
		Viewport.TopLeftX = Viewport.TopLeftY = 0.0f;
		DeviceContext->RSSetViewports(1, &Viewport);
	}

	void DX11Context::ResizeSwapChain(uint32_t width, uint32_t height)
	{
		OGN_PROFILER_RENDERING();

		if (DeviceContext) DeviceContext->OMSetRenderTargets(0, 0, 0);

		// Release all outstanding references to the swap chain's buffers.
		if (RenderTargetView) RenderTargetView->Release();

		// Preserve the existing buffer count and format
		SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

		ID3D11Texture2D *backBuffer = NULL;
		SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&backBuffer));
		Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);
		backBuffer->Release();

		DeviceContext->OMSetRenderTargets(1, &RenderTargetView, NULL);

		memset(&Viewport, 0, sizeof(D3D11_VIEWPORT));
		Viewport.Width = static_cast<FLOAT>(width);
		Viewport.Height = static_cast<FLOAT>(height);
		Viewport.MinDepth = 0.0f;
		Viewport.MaxDepth = 1.0f;
		Viewport.TopLeftX = Viewport.TopLeftY = 0.0f;
		DeviceContext->RSSetViewports(1, &Viewport);
	}

	void DX11Context::CleanupDevice()
	{
		OGN_PROFILER_RENDERING();

		if (RenderTargetView)
		{
			RenderTargetView->Release();
			RenderTargetView = nullptr;
		}

		if (SwapChain)
		{
			SwapChain->Release();
			SwapChain = nullptr;
		}

		if (Device)
		{
			Device->Release();
			Device = nullptr;
		}

		if (DeviceContext)
		{
			DeviceContext->Release();
			DeviceContext = nullptr;
		}
	}
}
