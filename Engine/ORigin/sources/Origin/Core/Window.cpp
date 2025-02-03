// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Window.h"

#include "KeyEvent.h"
#include "MouseEvent.h"
#include "AppEvent.h"
#include "Application.h"

#include "stb_image.h"
#include "Platform/Vulkan/VulkanContext.hpp"

#include <GLFW/glfw3.h>

#ifdef OGN_PLATFORM_WINDOWS
#include <GLFW/glfw3native.h>
#include <Windows.h>
#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")  // Link to DWM API
#endif

namespace origin
{
static void GlfwErrorCallback(int errorCode, const char* description)
{
    OGN_CORE_ASSERT(false, "[glfw] [{}]: {}", errorCode, description);
}

Window::Window(const char* title, u32 width, u32 height, bool maximized)
{
    OGN_PROFILER_FUNCTION();

    i32 success = glfwInit();
    glfwSetErrorCallback(GlfwErrorCallback);
    if (!success)
    {
        OGN_CORE_ASSERT(false, "[glfwInit] Failed to initialize GLFW");
    }

    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::OpenGL:
#if defined(OGN_DEBUG)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
        break;
    case RendererAPI::API::Vulkan:
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_is_vulkan = true;
        break;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    m_handle = glfwCreateWindow(
        static_cast<i32>(width),
        static_cast<i32>(height),
        title, nullptr, nullptr
    );

    if (!m_handle)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

#ifdef OGN_PLATFORM_WINDOWS
    HWND hwnd = glfwGetWin32Window(m_handle);
    BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

    // 7160E8 visual studio purple
    COLORREF rgbRed = 0x00E86071;
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &rgbRed, sizeof(rgbRed));

    // DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_DONOTROUND;
    // DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));
#endif

    m_data.Width = width;
    m_data.Height = height;

    glfwHideWindow(m_handle);

    i32 monitor_width, monitor_height;
    glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), nullptr, nullptr, &monitor_width, &monitor_height);

    m_data.xPos = (monitor_width / 2) - (width / 2);
    m_data.yPos = (monitor_height / 2) - (height / 2);
    glfwSetWindowPos(m_handle, m_data.xPos, m_data.yPos);

    i32 fb_width, fb_height;
    glfwGetFramebufferSize(m_handle, &fb_width, &fb_height);
    m_data.Width = static_cast<u32>(fb_width);
    m_data.FbHeight = static_cast<u32>(fb_height);

    if (!m_is_vulkan)
    {
        glfwMakeContextCurrent(m_handle);
        glfwSwapInterval(m_data.VSync ? 1 : 0);
    }

    m_graphics_context = GraphicsContext::Create();
    m_graphics_context->Init(this);

    if (maximized)
    {
        Maximize();
    }
}

Window::~Window()
{
    m_graphics_context->Shutdown();
    glfwDestroyWindow(m_handle);
    m_handle = nullptr;

    glfwTerminate();
}

void Window::Show()
{
    glfwShowWindow(m_handle);
}

void Window::Hide()
{
    glfwHideWindow(m_handle);
}

void Window::SetEventCallback(const std::function<void(Event&)>& callback)
{
    OGN_PROFILER_FUNCTION();

    m_data.EventCallback = callback;
    WindowCallbacks();
}

void Window::SetPosition(i32 x, i32 y)
{
    glfwSetWindowPos(m_handle, x, y);
}

glm::ivec2 Window::GetPosition()
{
    i32 width, height;
    glfwGetWindowPos(m_handle, &width, &height);
    return { width, height };
}

void Window::Maximize()
{
    glfwMaximizeWindow(m_handle);
    m_data.Maximized = true;

    i32 width, height;
    glfwGetWindowSize(m_handle, &width, &height);
    m_data.Width = static_cast<u32>(width);
    m_data.Height = static_cast<u32>(height);
}

void Window::Minimize()
{
}

u32 Window::GetFramebufferWidth() const
{
    return m_data.FbWidth;
}

u32 Window::GetFramebufferHeight() const
{
    return m_data.FbHeight;
}

void Window::UpdateEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    switch (RendererAPI::GetAPI())
    {
    case RendererAPI::API::OpenGL:
        glfwSwapBuffers(m_handle);
        break;
    case RendererAPI::API::Vulkan:
        VulkanContext::GetInstance()->Present();
        break;
    }
}

bool Window::IsLooping()
{
    return glfwWindowShouldClose(m_handle) == 0;
}

void Window::ToggleVSync()
{
    OGN_PROFILER_FUNCTION();

    if (!m_is_vulkan)
    {
        m_data.VSync = !m_data.VSync;
        glfwSwapInterval(m_data.VSync ? 1 : 0);
    }
}

void Window::SetVSync(bool enable)
{
    if (!m_is_vulkan)
    {
        m_data.VSync = enable;
        glfwSwapInterval(enable ? 1 : 0);
    }
}

void Window::CloseWindow()
{
    glfwSetWindowShouldClose(m_handle, 1);
}

void Window::ToggleFullScreen()
{
    OGN_PROFILER_FUNCTION();

    m_data.FullScreen = !m_data.FullScreen;
    if (m_data.FullScreen)
    {
        i32 width, height;
        glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), nullptr, nullptr, &width, &height);
        glfwSetWindowMonitor(m_handle, glfwGetPrimaryMonitor(), 0, 0, width, height, GLFW_DONT_CARE);
    }
    else
    {
        glfwSetWindowMonitor(m_handle, nullptr, m_data.xPos, m_data.yPos, m_data.Width, m_data.Height, GLFW_DONT_CARE);
        glfwSetWindowSize(m_handle, m_data.Width, m_data.Height);
    }
}

void Window::SetIcon(const char* filepath)
{
    OGN_PROFILER_FUNCTION();

    stbi_set_flip_vertically_on_load(0);
    i32 width, height, bpp;
    unsigned char* data = stbi_load(filepath, &width, &height, &bpp, 0);

    GLFWimage icon;
    icon.width = width;
    icon.height = height;
    icon.pixels = data;
    glfwSetWindowIcon(m_handle, 1, &icon);

    stbi_image_free(data);
}
void Window::SetIcon(unsigned char* data, u32 width, u32 height)
{
    GLFWimage icon;
    icon.width = static_cast<i32>(width);
    icon.height = static_cast<i32>(height);
    icon.pixels = data;
    glfwSetWindowIcon(m_handle, 1, &icon);
}

void Window::WindowCallbacks()
{
    OGN_PROFILER_FUNCTION();

    glfwSetWindowUserPointer(m_handle, &m_data);

    glfwSetWindowMaximizeCallback(m_handle, [](GLFWwindow* window, int maximized)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.Maximized = (bool)maximized;
    });

    glfwSetWindowPosCallback(m_handle, [](GLFWwindow* window, int xpos, int ypos)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.xPos = xpos;
        data.yPos = ypos;
    });

    glfwSetFramebufferSizeCallback(m_handle, [](GLFWwindow* window, int width, int height)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        data.FbWidth = static_cast<u32>(width);
        data.FbHeight = static_cast<u32>(height);

        FramebufferResizeEvent event(width, height);
        data.EventCallback(event);
    });

    glfwSetWindowSizeCallback(m_handle, [](GLFWwindow* window, int width, int height)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;

        WindowResizeEvent event(width, height);
        data.EventCallback(event);
    });

    glfwSetWindowCloseCallback(m_handle, [](GLFWwindow* window)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        WindowCloseEvent event;
        data.EventCallback(event);
    });

    glfwSetKeyCallback(m_handle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action)
        {
        case GLFW_PRESS:
        {
            KeyPressedEvent event(key, 0);
            data.EventCallback(event);
            break;
        }

        case GLFW_RELEASE:
        {
            KeyReleasedEvent event(key);
            data.EventCallback(event);
            break;
        }

        case GLFW_REPEAT:
        {
            KeyPressedEvent event(key, 1);
            data.EventCallback(event);
            break;
        }
        }
    });

    glfwSetCharCallback(m_handle, [](GLFWwindow* window, unsigned int keycode)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        KeyTypedEvent event(keycode);
        data.EventCallback(event);
    });

    glfwSetMouseButtonCallback(m_handle, [](GLFWwindow* window, int button, int action, int mods)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        switch (action)
        {
        case GLFW_PRESS:
        {
            MouseButtonPressedEvent event(button);
            data.EventCallback(event);
            break;
        }
        case GLFW_RELEASE:
        {
            MouseButtonReleasedEvent event(button);
            data.EventCallback(event);
            break;
        }
        }
    });

    glfwSetScrollCallback(m_handle, [](GLFWwindow* window, f64 x_offset, f64 y_offset)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseScrolledEvent event((float)x_offset, (float)y_offset);
        data.EventCallback(event);
    });

    glfwSetCursorPosCallback(m_handle, [](GLFWwindow* window, f64 xPos, f64 yPos)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        MouseMovedEvent event((float)xPos, (float)yPos);
        data.EventCallback(event);
    });

    glfwSetDropCallback(m_handle, [](GLFWwindow* window, int path_count, const char* paths[])
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
        std::vector<std::filesystem::path> filepaths(path_count);

        OGN_CORE_INFO("Paths: {}");
        for (int i = 0; i < path_count; i++)
        {
            filepaths[i] = paths[i];
            OGN_CORE_INFO(" {}", filepaths[i]);
        }

        WindowDropEvent event(std::move(filepaths));
        data.EventCallback(event);
    });
}
}