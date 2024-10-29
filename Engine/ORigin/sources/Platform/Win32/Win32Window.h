// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Core/Window.h"
#include "Origin/Renderer/GraphicsContext.h"

#include <string>

namespace origin
{
    class Win32Window : public Window
    {
    public:
        Win32Window(const char* title, uint32_t width, uint32_t height, bool maximized);

        void Show() override;
        void Hide() override;

        void DestroyWindow() override;
        void UpdateEvents() override;
        void OnUpdate() override;
        bool IsLooping() override { return glfwWindowShouldClose(m_MainWindow) == 0; }
        void CloseWindow() override;
        void ToggleVSync() override;
        void SetVSync(bool enable) override;
        void ToggleFullScreen() override;
        void SetIcon(const char* filepath) override;
        void WindowCallbacks();
        void SetEventCallback(const std::function<void(Event&)>& callback) override;

        const char* GetTitle() const override { return m_Data.Title.c_str(); }
        uint32_t GetWidth() const override { return m_Data.Width; }
        uint32_t GetHeight() const override { return m_Data.Height; }
        void* GetNativeWindow() override { return m_MainWindow; }

    private:
        WindowData m_Data;
        GLFWwindow* m_MainWindow;
        std::shared_ptr<GraphicsContext> m_GraphicsContext;
    };
}


