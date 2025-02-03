// Copyright (c) Evangelion Manuhutu | ORigin Engine

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
    std::function<void(Event&)> EventCallback;
};

class Window
{
public:
    Window(const char* title, u32 width, u32 height, bool maximized);
    ~Window();

    void Show();
    void Hide();

    void UpdateEvents();
    void SwapBuffers();
    bool IsLooping();
    void CloseWindow();
    void ToggleVSync();
    void SetVSync(bool enable);
    void ToggleFullScreen();
    void SetIcon(const char* filepath);
    void SetIcon(unsigned char* data, u32 width, u32 height);
    void WindowCallbacks();
    void SetEventCallback(const std::function<void(Event&)>& callback);
    void SetPosition(i32 x, i32 y);
    glm::ivec2 GetPosition();
    void Maximize();
    void Minimize();

    const char* GetTitle() const { return m_data.Title.c_str(); }
    u32 GetWidth() const { return m_data.Width; }
    u32 GetHeight() const { return m_data.Height; }

    virtual u32 GetFramebufferWidth() const;
    virtual u32 GetFramebufferHeight() const;

    GLFWwindow* GetNativeWindow() { return m_handle; }

private:
    WindowData m_data;
    GLFWwindow* m_handle;
    Ref<GraphicsContext> m_graphics_context;
    bool m_is_vulkan = false;
};

}

#endif