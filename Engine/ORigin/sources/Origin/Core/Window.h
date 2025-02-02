// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef WINDOW_H
#define WINDOW_H

#include "Event.h"

#ifdef OGN_PLATFORM_WINDOWS
    #include <Windows.h>
#endif

#define GLFW_INVLUDE_VULKAN
#include <GLFW/glfw3.h>

struct GLFWwindow;
namespace origin {
    class GraphicsContext;

    struct WindowData
    {
        std::string Title;
        u32 Width, Height;
        u32 FbWidth, FbHeight;
        int xPos, yPos;
        bool Maximized = false;
        bool FullScreen = false;
        bool VSync = true;
        std::function<void(Event &)> EventCallback;
    };
    
    class Window
    {
    public:
        static void GLFWInit();
        static void GLFWShutdown();
        static void Show(GLFWwindow *window);
        static void Hide(GLFWwindow *window);

        virtual void Show() = 0;
        virtual void Hide() = 0;
        virtual void DestroyWindow() = 0;
        virtual void UpdateEvents() = 0;
        virtual void SwapBuffers() = 0;
        virtual bool IsLooping() = 0;
        virtual void ToggleVSync() = 0;
        virtual void SetVSync(bool enable) = 0;
        virtual void ToggleFullScreen() = 0;
        virtual void CloseWindow() = 0;
        virtual void SetIcon(const char* filepath) = 0;
        virtual void SetIcon(unsigned char* data, u32 width, u32 height) = 0;
        virtual void SetEventCallback(const std::function<void(Event&)>& callback) = 0;
        virtual void SetPosition(int x, int y) = 0;
        virtual glm::ivec2 GetPosition() = 0;
        virtual void Maximize() = 0;
        virtual void Minimize() = 0;

        virtual u32 GetWidth() const = 0;
        virtual u32 GetHeight() const = 0;
        virtual u32 GetFramebufferWidth() const = 0;
        virtual u32 GetFramebufferHeight() const = 0;
        virtual const char* GetTitle() const = 0;
        virtual GLFWwindow *GetNativeWindow() = 0;

        static Ref<Window> Create(const char *title, uint32_t width, uint32_t height, bool maximized);
    protected:
        bool m_IsVulkanAPI = false;
    };
}

#endif