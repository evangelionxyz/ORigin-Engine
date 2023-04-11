// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\Window.h"
#include "Origin\IO\Events\Event.h"
#include "Origin\Renderer\GraphicsContext.h"

#include <string>

namespace Origin
{
	template<typename T>
	struct WndVec2 { T x; T y; };

	class WinWindow : public Window
	{
	public:
		WinWindow(const std::string& title);
		virtual ~WinWindow();

		void SetVSync(bool enable = false) override;
		void SetClose(bool close = false) override;

		void Init();
		void SetEventCallback(const std::function<void(Event&)>& callback) override {
			m_Data.EventCallback = callback; };

		void OnUpdate() override;
		bool Loop() const override { return glfwWindowShouldClose(m_Window) == m_Data.Close; }
		void SetFullscreen(bool enable) override { m_Data.Fullscreen = enable; }

		void SetIcon(const std::string& filepath) override;
		void WindowCallbacks();

		uint32_t GetWidth() const override { return m_Data.Width; }
		uint32_t GetHeight() const override { return m_Data.Height; }
		std::string GetTitle() const override { return m_Data.Title; }
		GLFWwindow* GetMainWindow() override { return m_Window; }

	private:
		struct WindowData
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;
			int xPos;
			int yPos;
			bool Maximized = false;
			bool Fullscreen = false;
			bool VSync = false;
			int Close = 0;

			std::function<void(Event&)> EventCallback;
		} m_Data;

		std::unique_ptr<GraphicsContext> m_Context;
		GLFWwindow* m_Window;
		GLFWmonitor* m_Monitor;
		WndVec2<int> monitorSize;
		WndVec2<int> monitorPos;
		WndVec2<int> windowSize;
		WndVec2<int> windowPos;
	};
}



