// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "GraphicsContext.h"
#include "Origin/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.hpp"

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
        }

        OGN_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
}
