// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Core/Window.h"
#include "Origin/Renderer/GraphicsContext.h"

#include "GLFW/glfw3.h"

#include <string>

namespace origin
{
    class Win32Window : public Window
    {
    public:
        Win32Window(const char* title, u32 width, u32 height, bool maximized);

        void Show() override;
        void Hide() override;

        void DestroyWindow() override;
        void UpdateEvents() override;
        void OnUpdate() override;
        bool IsLooping() override;
        void CloseWindow() override;
        void ToggleVSync() override;
        void SetVSync(bool enable) override;
        void ToggleFullScreen() override;
        void SetIcon(const char *filepath) override;
        void SetIcon(unsigned char *data, u32 width, u32 height) override;
        void WindowCallbacks();
        void SetEventCallback(const std::function<void(Event&)>& callback) override;
        void SetPosition(int x, int y) override;
        glm::ivec2 GetPosition() override;
        void Maximize() override;
        void Minimize() override;

        const char* GetTitle() const override { return m_Data.Title.c_str(); }
        u32 GetWidth() const override { return m_Data.Width; }
        u32 GetHeight() const override { return m_Data.Height; }

        virtual u32 GetFramebufferWidth() const override;
        virtual u32 GetFramebufferHeight() const override;

        GLFWwindow *GetNativeWindow() override { return m_MainWindow; }

    private:
        WindowData m_Data;
        GLFWwindow* m_MainWindow;
        Ref<GraphicsContext> m_GraphicsContext;
    };
}


