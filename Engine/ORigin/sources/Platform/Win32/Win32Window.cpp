// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Win32Window.h"

#include "Origin/Core/KeyEvent.h"
#include "Origin/Core/MouseEvent.h"
#include "Origin/Core/AppEvent.h"
#include "Origin/Core/Application.h"

#include "stb_image.h"
#include "Platform/DX11/DX11Context.h"
#include "Platform/Vulkan/VulkanContext.hpp"

#include <GLFW/glfw3.h>

#ifdef OGN_PLATFORM_WINDOWS
    #include <GLFW/glfw3native.h>
    #include <Windows.h>
    #include <dwmapi.h>
    #pragma comment(lib, "Dwmapi.lib")  // Link to DWM API
#endif

namespace origin {
Win32Window::Win32Window(const char* title, uint32_t width, uint32_t height, bool maximized)
{
    OGN_PROFILER_FUNCTION();

    switch (Renderer::GetAPI())
    {
    case RendererAPI::API::OpenGL:
#if defined(OGN_DEBUG)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
        break;
    case RendererAPI::API::Vulkan:
        m_IsVulkanAPI = true;
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        break;
    }
        
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    m_MainWindow = glfwCreateWindow(
        static_cast<i32>(width), 
        static_cast<i32>(height),
        title, nullptr, nullptr
    );

    if (!m_MainWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

#ifdef OGN_PLATFORM_WINDOWS
    HWND hwnd = glfwGetWin32Window(m_MainWindow);
    BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

    // 7160E8 visual studio purple
    COLORREF rgbRed = 0x00E86071;
    DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &rgbRed, sizeof(rgbRed));

    // DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_DONOTROUND;
    // DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));
#endif

    m_Data.Width = width;
    m_Data.Height = height;

    glfwHideWindow(m_MainWindow);

    int monitor_width, monitor_height;
    glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), nullptr, nullptr, &monitor_width, &monitor_height);

    m_Data.xPos = (monitor_width / 2) - (width / 2);
    m_Data.yPos = (monitor_height / 2) - (height / 2);
    glfwSetWindowPos(m_MainWindow, m_Data.xPos, m_Data.yPos);

    i32 fb_width, fb_height;
    glfwGetFramebufferSize(m_MainWindow, &fb_width, &fb_height);
    m_Data.Width = static_cast<u32>(fb_width);
    m_Data.FbHeight = static_cast<u32>(fb_height);

    if (!m_IsVulkanAPI)
    {
        glfwMakeContextCurrent(m_MainWindow);
        glfwSwapInterval(m_Data.VSync ? 1 : 0);
    }

    m_GraphicsContext = GraphicsContext::Create();
    m_GraphicsContext->Init(this);

    if (maximized)
    {
        Maximize();
    }
}

void Win32Window::Show()
{
    glfwShowWindow(m_MainWindow);
}

void Win32Window::Hide()
{
    glfwHideWindow(m_MainWindow);
}

void Win32Window::DestroyWindow()
{
    m_GraphicsContext->Shutdown();
    glfwDestroyWindow(m_MainWindow);
}

void Win32Window::SetEventCallback(const std::function<void(Event&)>& callback)
{
    OGN_PROFILER_FUNCTION();

    m_Data.EventCallback = callback;
    WindowCallbacks();
}

void Win32Window::SetPosition(i32 x, i32 y)
{
    glfwSetWindowPos(m_MainWindow, x, y);
}

glm::ivec2 Win32Window::GetPosition()
{
    i32 width, height;
    glfwGetWindowPos(m_MainWindow, &width, &height);
    return { width, height };
}

void Win32Window::Maximize()
{
    glfwMaximizeWindow(m_MainWindow);
    m_Data.Maximized = true;

    i32 width, height;
    glfwGetWindowSize(m_MainWindow, &width, &height);
    m_Data.Width = static_cast<u32>(width);
    m_Data.Height = static_cast<u32>(height);
}

void Win32Window::Minimize()
{
}

u32 Win32Window::GetFramebufferWidth() const
{
    return m_Data.FbWidth;
}

u32 Win32Window::GetFramebufferHeight() const
{
    return m_Data.FbHeight;
}

void Win32Window::UpdateEvents()
{
    glfwPollEvents();
}

void Win32Window::OnUpdate()
{
    switch (RendererAPI::GetAPI())
    {
    case RendererAPI::API::DX11:
    {
        Ref<DX11Context> dx_context = GraphicsContext::GetContext<DX11Context>();
        dx_context->GetSwapchain()->Present(1u, 0u);
        break;
    }
    case RendererAPI::API::OpenGL:
    {
        glfwSwapBuffers(m_MainWindow);
        break;
    }
    case RendererAPI::API::Vulkan:
    {
        GraphicsContext::GetContext<VulkanContext>()->Present();
        break;
    }
    }
}

bool Win32Window::IsLooping()
{
    return glfwWindowShouldClose(m_MainWindow) == 0;
}

void Win32Window::ToggleVSync()
{
    OGN_PROFILER_FUNCTION();

    if (!m_IsVulkanAPI)
    {
        m_Data.VSync = !m_Data.VSync;
        glfwSwapInterval(m_Data.VSync ? 1 : 0);
    }
}

void Win32Window::SetVSync(bool enable)
{
    if (!m_IsVulkanAPI)
    {
        m_Data.VSync = enable;
        glfwSwapInterval(enable ? 1 : 0);
    }
}

void Win32Window::CloseWindow()
{
    glfwSetWindowShouldClose(m_MainWindow, 1);
}

void Win32Window::ToggleFullScreen()
{
    OGN_PROFILER_FUNCTION();

    m_Data.FullScreen = !m_Data.FullScreen;
    if (m_Data.FullScreen)
    {
        i32 width, height;
        glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), nullptr, nullptr, &width, &height);
        glfwSetWindowMonitor(m_MainWindow, glfwGetPrimaryMonitor(), 0, 0, width, height, GLFW_DONT_CARE);
    }
    else
    {
        glfwSetWindowMonitor(m_MainWindow, nullptr, m_Data.xPos, m_Data.yPos, m_Data.Width, m_Data.Height, GLFW_DONT_CARE);
        glfwSetWindowSize(m_MainWindow, m_Data.Width, m_Data.Height);
    }
}

void Win32Window::SetIcon(const char* filepath)
{
    OGN_PROFILER_FUNCTION();

    stbi_set_flip_vertically_on_load(0);
    i32 width, height, bpp;
    unsigned char* data = stbi_load(filepath, &width, &height, &bpp, 0);

    GLFWimage icon;
    icon.width = width;
    icon.height = height;
    icon.pixels = data;
    glfwSetWindowIcon(m_MainWindow, 1, &icon);

    stbi_image_free(data);
}
void Win32Window::SetIcon(unsigned char* data, u32 width, u32 height)
{
    GLFWimage icon;
    icon.width = static_cast<i32>(width);
    icon.height = static_cast<i32>(height);
    icon.pixels = data;
    glfwSetWindowIcon(m_MainWindow, 1, &icon);
}

void Win32Window::WindowCallbacks()
{
    OGN_PROFILER_FUNCTION();

    glfwSetWindowUserPointer(m_MainWindow, &m_Data);

    glfwSetWindowMaximizeCallback(m_MainWindow, [](GLFWwindow* window, int maximized)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Maximized = (bool)maximized;
        });

    glfwSetWindowPosCallback(m_MainWindow, [](GLFWwindow* window, int xpos, int ypos)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.xPos = xpos;
            data.yPos = ypos;
        });

    glfwSetFramebufferSizeCallback(m_MainWindow, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            data.FbWidth = static_cast<u32>(width);
            data.FbHeight = static_cast<u32>(height);

            FramebufferResizeEvent event(width, height);
            data.EventCallback(event);
        });

    glfwSetWindowSizeCallback(m_MainWindow, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

    glfwSetWindowCloseCallback(m_MainWindow, [](GLFWwindow* window)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });

    glfwSetKeyCallback(m_MainWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
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

    glfwSetCharCallback(m_MainWindow, [](GLFWwindow* window, unsigned int keycode)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            KeyTypedEvent event(keycode);
            data.EventCallback(event);
        });

    glfwSetMouseButtonCallback(m_MainWindow, [](GLFWwindow* window, int button, int action, int mods)
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

    glfwSetScrollCallback(m_MainWindow, [](GLFWwindow* window, f64 x_offset, f64 y_offset)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseScrolledEvent event((float)x_offset, (float)y_offset);
            data.EventCallback(event);
        });

    glfwSetCursorPosCallback(m_MainWindow, [](GLFWwindow* window, f64 xPos, f64 yPos)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });

    glfwSetDropCallback(m_MainWindow, [](GLFWwindow* window, int path_count, const char* paths[])
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