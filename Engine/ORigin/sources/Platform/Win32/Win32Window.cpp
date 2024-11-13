// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Win32Window.h"

#include "Origin/Core/KeyEvent.h"
#include "Origin/Core/MouseEvent.h"
#include "Origin/Core/AppEvent.h"
#include "Origin/Core/Application.h"

#include "stb_image.h"
#include "Platform/DX11/DX11Context.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <Windows.h>
#include <dwmapi.h>

#pragma comment(lib, "Dwmapi.lib")  // Link to DWM API

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
        }
        
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_MAXIMIZED, (int)maximized);
        // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

        m_MainWindow = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height),
            title, nullptr, nullptr);

        if (!m_MainWindow)
        {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        HWND hwnd = glfwGetWin32Window(m_MainWindow);
        BOOL useDarkMode = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDarkMode, sizeof(useDarkMode));

        // 7160E8 visual studio purple
        COLORREF rgbRed = 0x00E86071;
        DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &rgbRed, sizeof(rgbRed));

        // DWM_WINDOW_CORNER_PREFERENCE preference = DWMWCP_DONOTROUND;
        // DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &preference, sizeof(preference));

        m_Data.Width = width;
        m_Data.Height = height;

        glfwHideWindow(m_MainWindow);

        int monitorWidth, monitorHeight;
        glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), nullptr, nullptr, &monitorWidth, &monitorHeight);

        m_Data.xPos = (monitorWidth / 2) - (width / 2);
        m_Data.yPos = (monitorHeight / 2) - (height / 2);
        glfwSetWindowPos(m_MainWindow, m_Data.xPos, m_Data.yPos);
        glfwMakeContextCurrent(m_MainWindow);

        m_GraphicsContext = GraphicsContext::Create();
        m_GraphicsContext->Init(this);

        glfwSwapInterval(m_Data.VSync ? 1 : 0);
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

    void Win32Window::SetPosition(int x, int y)
    {
        glfwSetWindowPos(m_MainWindow, x, y);
    }

    glm::ivec2 Win32Window::GetPosition()
    {
        int width, height;
        glfwGetWindowPos(m_MainWindow, &width, &height);
        return { width, height };
    }

    void Win32Window::Maximize()
    {
        glfwMaximizeWindow(m_MainWindow);
    }

    void Win32Window::Minimize()
    {
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
            DX11Context *dx11_context = DX11Context::GetInstance();
            dx11_context->SwapChain->Present(1u, 0u);
            break;
        }

        case RendererAPI::API::OpenGL:
            glfwSwapBuffers(m_MainWindow);
            break;
        }
    }

    void Win32Window::ToggleVSync()
    {
        OGN_PROFILER_FUNCTION();

        m_Data.VSync = !m_Data.VSync;
        glfwSwapInterval(m_Data.VSync ? 1 : 0);
    }

    void Win32Window::SetVSync(bool enable)
    {
        m_Data.VSync = enable;
        glfwSwapInterval(enable ? 1 : 0);
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
            int width, height;
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
        int width, height, bpp;
        unsigned char* data = stbi_load(filepath, &width, &height, &bpp, 0);

        GLFWimage icon;
        icon.width = width;
        icon.height = height;
        icon.pixels = data;
        glfwSetWindowIcon(m_MainWindow, 1, &icon);

        stbi_image_free(data);
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

        glfwSetScrollCallback(m_MainWindow, [](GLFWwindow* window, double xOffset, double yOffset)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                MouseScrolledEvent event((float)xOffset, (float)yOffset);
                data.EventCallback(event);
            });

        glfwSetCursorPosCallback(m_MainWindow, [](GLFWwindow* window, double xPos, double yPos)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                MouseMovedEvent event((float)xPos, (float)yPos);
                data.EventCallback(event);
            });

        glfwSetDropCallback(m_MainWindow, [](GLFWwindow* window, int pathCount, const char* paths[])
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                std::vector<std::filesystem::path> filepaths(pathCount);
                for (int i = 0; i < pathCount; i++)
                    filepaths[i] = paths[i];

                WindowDropEvent event(std::move(filepaths));
                data.EventCallback(event);
            });
    }
}