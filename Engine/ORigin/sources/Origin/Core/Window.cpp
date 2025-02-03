// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Window.h"

#include "Platform/Win32Window.h"

namespace origin
{
    static void GlfwErrorCallback(int errorCode, const char *description)
    {
        OGN_CORE_ASSERT(false, "[glfw] [{}]: {}", errorCode, description);
    }

    Ref<Window> Window::Create(const char* title, uint32_t width, uint32_t height, bool maximized)
    {
        return CreateRef<Win32Window>(title, width, height, maximized);
    }

    void Window::GLFWInit()
    {
        int success = glfwInit();
        glfwSetErrorCallback(GlfwErrorCallback);
        if(!success)
        {
            OGN_CORE_ASSERT(false, "[glfwInit] Failed to initialize GLFW");
        }
    }

    void Window::GLFWShutdown() 
    {
        glfwTerminate();
    }

    void Window::Show(GLFWwindow *window)
    {
        glfwShowWindow(window);
    }

    void Window::Hide(GLFWwindow *window)
    {
        glfwHideWindow(window);
    }

}