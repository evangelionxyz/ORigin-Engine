// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef LINUX_WINDOW_H
#define LINUX_WINDOW_H

#include "Origin/Core/Window.h"

namespace origin
{
    class LinuxWindow : public Window
    {
	public:
        LinuxWindow(const char* title, uint32_t width, uint32_t height, bool maximized);
		virtual ~LinuxWindow() override;

		void OnUpdate() override;
		bool IsLooping() override { return glfwWindowShouldClose(m_MainWindow) == 0; }
		void ToggleVSync() override;
		void ToggleFullScreen() override;
		void CloseWindow() override;
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

#endif