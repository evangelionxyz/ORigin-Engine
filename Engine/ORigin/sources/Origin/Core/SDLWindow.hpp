#pragma once

#include "Event.h"

#include "Origin/Renderer/GraphicsContext.h"
#include <SDL3/SDL.h>

namespace origin
{

class SDLWindow
{
public:
    SDLWindow(const char *title, u32 width, u32 height, bool maximized);
    ~SDLWindow();

    void Show();
    void Hide();

    void UpdateEvents();
    void SwapBuffers();
    bool IsLooping();
    void CloseWindow();
    void ToggleVSync();
    void SetVSync(bool enable);
    void ToggleFullScreen();
    void SetIcon(const char *filepath);
    void SetIcon(unsigned char *data, u32 width, u32 height);
    void AddSDLEventCallback(const std::function<void(const SDL_Event *)> &callback);
    void SetEventCallback(const std::function<void(Event &)> &callback);
    void SetPosition(i32 x, i32 y);
    glm::ivec2 GetPosition();
    void Maximize();
    void Minimize();

    const char *GetTitle() const { return m_data.Title.c_str(); }
    u32 GetWidth() const { return m_data.Width; }
    u32 GetHeight() const { return m_data.Height; }

    virtual u32 GetFramebufferWidth() const;
    virtual u32 GetFramebufferHeight() const;

    SDL_Window *GetNativeWindow() { return m_handle; }
    SDL_GLContext GetGLContext() { return m_gl_context; }
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
        std::vector<std::function<void(const SDL_Event *)>> SDLEventCallbacks;
    };

private:
    WindowData m_data;
    SDL_Window *m_handle = nullptr;
    SDL_GLContext m_gl_context = nullptr;
    SDL_Event m_event;

    bool m_is_running = true;

    Ref<GraphicsContext> m_graphics_context;
    bool m_is_vulkan = false;
};


}