// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef WINDOW_H
#define WINDOW_H

#include "Event.h"

#ifdef OGN_PLATFORM_WINDOWS
    #include <Windows.h>
#endif

#define GLFW_INVLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace origin {

    class GraphicsContext;

    struct WindowData
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;
        int xPos;
        int yPos;
        bool Maximized = false;
        bool FullScreen = false;
        bool VSync = false;
        std::function<void(Event &)> EventCallback;
    };
    
    class Window
    {
    public:
        static void GLFWInit();
        static void GLFWShutdown();

        virtual void DestroyWindow() = 0;
        virtual void UpdateEvents() = 0;
        virtual void OnUpdate() = 0;
        virtual bool IsLooping() = 0;
        virtual void ToggleVSync() = 0;
        virtual void ToggleFullScreen() = 0;
        virtual void CloseWindow() = 0;
        virtual void SetIcon(const char* filepath) = 0;
        virtual void SetEventCallback(const std::function<void(Event&)>& callback) = 0;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual const char* GetTitle() const = 0;
        virtual void *GetNativeWindow() { return nullptr; }
        static std::shared_ptr<Window> Create(const char *title, uint32_t width, uint32_t heigth, bool maximized = false);
    };
}

#endif