// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\Window.h"
#include "Origin\Renderer\GraphicsContext.h"
#include <string>

namespace origin
{
	struct WindowData
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		int xPos;
		int yPos;
		bool Maximized = false;
		bool VSync = false;
		int Close = 0;
		std::function<void(Event&)> EventCallback;
	};

	class WinWindow : public Window
	{
	public:

		WinWindow(const char* title, uint32_t width, uint32_t height, bool maximized);
		virtual ~WinWindow() override;
		void OnUpdate() override;
		bool IsLooping() const override { return glfwWindowShouldClose(m_MainWindow) == m_Data.Close; }

		void SetClose(bool close = false) override;
		void SetVSync(bool enable = false) override;
		void SetIcon(const char* filepath) override;
		void WindowCallbacks();
		void SetFullscreen(bool enable) override;
		void SetEventCallback(const std::function<void(Event&)>& callback) override;

		const char* GetTitle() const override { return m_Data.Title.c_str(); }
		uint32_t GetWidth() const override { return m_Data.Width; }
		uint32_t GetHeight() const override { return m_Data.Height; }
		GLFWwindow* GetNativeWindow() override { return m_MainWindow; }

	private:
		WindowData m_Data;
		GLFWwindow* m_MainWindow;
		std::shared_ptr<GraphicsContext> m_GraphicsContext;
	};
}



