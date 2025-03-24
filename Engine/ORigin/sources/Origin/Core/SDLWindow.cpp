#include "pch.h"

#include <glad/glad.h>

#include "SDLWindow.hpp"

#include "Application.h"
#include "AppEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "Input.h"

#include "Platform/Vulkan/VulkanContext.hpp"
#include "stb_image.h"

#ifdef OGN_PLATFORM_WINDOWS
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib") // Link to DWM API
#endif

namespace origin
{

SDLWindow::SDLWindow(const char *title, u32 width, u32 height, bool maximized)
{
    SDL_SetAppMetadata("origin", "v1.0", "origin");

    SDL_InitFlags init_flags = SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_CAMERA | SDL_INIT_AUDIO | SDL_INIT_HAPTIC | SDL_INIT_GAMEPAD;

    if (!SDL_Init(init_flags))
    {
        OGN_CORE_ASSERT(false, "[Window] {}", SDL_GetError());
    }

    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | (maximized ? SDL_WINDOW_MAXIMIZED : 0);

    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::OpenGL:
        m_is_vulkan = false;
        window_flags |= SDL_WINDOW_OPENGL;
        break;
    case RendererAPI::API::Vulkan:
        m_is_vulkan = true;
        window_flags |= SDL_WINDOW_VULKAN;
        break;
    }

    m_handle = SDL_CreateWindow(title, width, height, window_flags);

    if (!m_handle)
    {
        SDL_Quit();
        OGN_CORE_ASSERT(m_handle, "[Window] Failed to create window {}", SDL_GetError());
    }

    SDL_ShowWindow(m_handle);

    m_data.Width = width;
    m_data.Height = height;
    m_data.Maximized = maximized;
    m_data.Title = title;

    SDL_SetWindowPosition(m_handle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    m_graphics_context = GraphicsContext::Create();
    if (!m_is_vulkan)
    {
        m_gl_context = SDL_GL_CreateContext(m_handle);
    }    
    m_graphics_context->Init(this);

    if (maximized)
    {
        Maximize();
    }
}

SDLWindow::~SDLWindow()
{
    if (m_gl_context)
    {
        SDL_GL_DestroyContext(m_gl_context);
    }

    SDL_DestroyWindowSurface(m_handle);

    SDL_DestroyWindow(m_handle);
    SDL_Quit();

    m_handle = nullptr;
}

void SDLWindow::Show()
{
    SDL_ShowWindow(m_handle);
}

void SDLWindow::Hide()
{
    SDL_HideWindow(m_handle);
}

void SDLWindow::UpdateEvents()
{
    while (SDL_PollEvent(&m_event))
    {
        for (auto &callback : m_data.SDLEventCallbacks)
        {
            if (callback)
            {
                callback(&m_event);
            }
        }
        
        switch (m_event.type)
        {
        case SDL_EVENT_QUIT:
        {
            m_is_running = false;
            WindowCloseEvent ev;
            m_data.EventCallback(ev);
            break;
        }
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        {
            m_data.FbWidth = static_cast<u32>(m_event.window.data1);
            m_data.FbWidth = static_cast<u32>(m_event.window.data2);

            FramebufferResizeEvent ev(m_data.FbWidth, m_data.FbHeight);
            m_data.EventCallback(ev);
            break;
        }
        case SDL_EVENT_WINDOW_MOVED:
        {
            m_data.xPos = m_event.window.data1;
            m_data.yPos = m_event.window.data2;
            break;
        }
        case SDL_EVENT_KEY_DOWN:
        {
            Input::SetModifier(KeyMod::Shift, m_event.key.mod & SDL_KMOD_SHIFT);
            Input::SetModifier(KeyMod::Control, m_event.key.mod & SDL_KMOD_CTRL);
            Input::SetModifier(KeyMod::LeftAlt, m_event.key.mod & SDL_KMOD_LALT);
            Input::SetModifier(KeyMod::RightAlt, m_event.key.mod & SDL_KMOD_RALT);
            Input::SetModifier(KeyMod::LeftShift, m_event.key.mod & SDL_KMOD_LSHIFT);
            Input::SetModifier(KeyMod::RightShift, m_event.key.mod & SDL_KMOD_RSHIFT);
            Input::SetModifier(KeyMod::LeftControl, m_event.key.mod & SDL_KMOD_LCTRL);
            Input::SetModifier(KeyMod::RightControl, m_event.key.mod & SDL_KMOD_RCTRL);

            Input::key_codes[m_event.key.key] = true;
            KeyPressedEvent ev(m_event.key.key, m_event.key.mod, 0);
            m_data.EventCallback(ev);

            break;
        }
        case SDL_EVENT_KEY_UP:
        {
            Input::SetModifier(KeyMod::Shift, m_event.key.mod & SDL_KMOD_SHIFT);
            Input::SetModifier(KeyMod::Control, m_event.key.mod & SDL_KMOD_CTRL);
            Input::SetModifier(KeyMod::LeftAlt, m_event.key.mod & SDL_KMOD_LALT);
            Input::SetModifier(KeyMod::RightAlt, m_event.key.mod & SDL_KMOD_RALT);
            Input::SetModifier(KeyMod::LeftShift, m_event.key.mod & SDL_KMOD_LSHIFT);
            Input::SetModifier(KeyMod::RightShift, m_event.key.mod & SDL_KMOD_RSHIFT);
            Input::SetModifier(KeyMod::LeftControl, m_event.key.mod & SDL_KMOD_LCTRL);
            Input::SetModifier(KeyMod::RightControl, m_event.key.mod & SDL_KMOD_RCTRL);

            Input::key_codes[m_event.key.key] = false;
            KeyReleasedEvent ev(m_event.key.key, m_event.key.mod);
            m_data.EventCallback(ev);

            break;
        }
        case SDL_EVENT_TEXT_INPUT:
        {
            KeyTypedEvent ev(m_event.text.text[0]);
            m_data.EventCallback(ev);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            Input::mouse_codes[m_event.button.button] = true;
            MouseButtonPressedEvent ev(m_event.button.button);
            m_data.EventCallback(ev);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            Input::mouse_codes[m_event.button.button] = false;
            MouseButtonReleasedEvent ev(m_event.button.button);
            m_data.EventCallback(ev);
            break;
        }
        case SDL_EVENT_MOUSE_WHEEL:
        {
            MouseScrolledEvent ev(static_cast<f32>(m_event.wheel.x), static_cast<f32>(m_event.wheel.y));
            m_data.EventCallback(ev);
            break;
        }
        case SDL_EVENT_MOUSE_MOTION:
        {
            MouseMovedEvent ev(static_cast<f32>(m_event.motion.x), static_cast<f32>(m_event.motion.y));
            m_data.EventCallback(ev);
            break;
        }
        case SDL_EVENT_DROP_FILE:
        {
            std::vector<std::filesystem::path> filepaths;
            filepaths.emplace_back(m_event.drop.data);

            OGN_CORE_INFO("Dropped file: {}", m_event.drop.data);

            WindowDropEvent ev(std::move(filepaths));
            m_data.EventCallback(ev);

            break;
        }
        case SDL_EVENT_WINDOW_MAXIMIZED:
            break;
        }
    }
}

void SDLWindow::SwapBuffers()
{
    switch (RendererAPI::GetAPI())
    {
    case RendererAPI::API::OpenGL:
        SDL_GL_SwapWindow(m_handle);
        break;
    case RendererAPI::API::Vulkan:
        VulkanContext::GetInstance()->Present();
        break;
    }
}

bool SDLWindow::IsLooping()
{
    return m_is_running;
}

void SDLWindow::CloseWindow()
{
    m_is_running = false;
}

void SDLWindow::ToggleVSync()
{
    m_data.VSync = !m_data.VSync;
    SDL_SetWindowSurfaceVSync(m_handle, m_data.VSync ? 1 : 0);
}

void SDLWindow::SetVSync(bool enable)
{
    m_data.VSync = enable;
    SDL_SetWindowSurfaceVSync(m_handle, m_data.VSync ? 1 : 0);
}

void SDLWindow::ToggleFullScreen()
{
    m_data.FullScreen = !m_data.FullScreen;
    SDL_SetWindowFullscreen(m_handle, m_data.FullScreen);
}

void SDLWindow::SetIcon(const char *filepath)
{
    stbi_set_flip_vertically_on_load(0);
    i32 width, height, bpp;
    stbi_uc *data = stbi_load(filepath, &width, &height, &bpp, 4);

    if (!data)
    {
        OGN_CORE_ERROR("[Window] Failed to load icon {}", filepath);
        return;
    }

    SDL_Surface *icon_surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, data, width * 4);
    if (!icon_surface)
    {
        OGN_CORE_ERROR("[Window] Faile dto create SDL_Surface for icon");
        stbi_image_free(data);
        return;
    }

    SDL_SetWindowIcon(m_handle, icon_surface);
    SDL_DestroySurface(icon_surface);
    stbi_image_free(data);
}


void SDLWindow::SetIcon(unsigned char *data, u32 width, u32 height)
{
    SDL_Surface *icon_surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, data, width * 4);
    if (!icon_surface)
    {
        OGN_CORE_ERROR("[Window] Faile dto create SDL_Surface for icon");
        stbi_image_free(data);
        return;
    }

    SDL_SetWindowIcon(m_handle, icon_surface);
    SDL_DestroySurface(icon_surface);
}

void SDLWindow::AddSDLEventCallback(const std::function<void(const SDL_Event *)> &callback)
{
    m_data.SDLEventCallbacks.push_back(callback);

}

void SDLWindow::SetEventCallback(const std::function<void(Event &)> &callback)
{
    m_data.EventCallback = callback;
}

void SDLWindow::SetPosition(i32 x, i32 y)
{
    SDL_SetWindowPosition(m_handle, x, y);
}

glm::ivec2 SDLWindow::GetPosition()
{
    i32 x, y;
    SDL_GetWindowPosition(m_handle, &x, &y);
    return { x, y };
}

void SDLWindow::Maximize()
{
    SDL_MaximizeWindow(m_handle);
}

void SDLWindow::Minimize()
{
    SDL_MinimizeWindow(m_handle);
}

u32 SDLWindow::GetFramebufferWidth() const
{
    return m_data.FbWidth;
}

u32 SDLWindow::GetFramebufferHeight() const
{
    return m_data.FbHeight;
}

}