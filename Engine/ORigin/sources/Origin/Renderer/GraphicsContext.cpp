// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "GraphicsContext.h"
#include "Origin/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.hpp"

#ifdef OGN_PLATFORM_WINDOWS
    #include "Platform/DX11/DX11Context.h"
#endif

namespace origin
{
    Ref<GraphicsContext> GraphicsContext::s_Instance = nullptr;

    Ref<GraphicsContext> GraphicsContext::Create()
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    
                OGN_CORE_ASSERT(false, "[GraphicsContext] Please select renderer api"); return nullptr;
            case RendererAPI::API::OpenGL: 
            {
                s_Instance = CreateRef<OpenGLContext>();
                return s_Instance;
            }
            case RendererAPI::API::Vulkan:
            {
                s_Instance = CreateRef<VulkanContext>();
                return s_Instance;
            }
#ifdef OGN_PLATFORM_WINDOWS
            case RendererAPI::API::DX11: 
            {
                s_Instance = CreateRef<DX11Context>();
                return s_Instance;
            }
#endif
        }

        OGN_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
