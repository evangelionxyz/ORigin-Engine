// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Core\Window.h"
#include "Origin\IO\Events\Event.h"
#include "Origin\Renderer\GraphicsContext.h"

#include <string>

namespace origin
{
	class WinWindow : public Window
	{
	public:

		WinWindow(const WindowConfig& config);
		WinWindow(const std::string& title, uint32_t width, uint32_t height);
		virtual ~WinWindow();

		void SetVSync(bool enable = false) override;
		void SetClose(bool close = false) override;

		void Init();
		void SetEventCallback(const std::function<void(Event&)>& callback) override;

		void OnUpdate() override;
		bool Loop() const override { return glfwWindowShouldClose(m_Window) == m_Data.Close; }
		void SetFullscreen(bool enable) override { m_Data.Fullscreen = enable; }

		void SetIcon(const std::string& filepath) override;
		void WindowCallbacks();

		void Destroy() override;
		void Decorated(bool enable) override;

		void SetSize(uint32_t width, uint32_t height) override;

		glm::uvec2 GetPosition() override { return { m_Data.xPos, m_Data.yPos }; }
		uint32_t GetWidth() const override { return m_Data.Width; }
		uint32_t GetHeight() const override { return m_Data.Height; }
		std::string GetTitle() const override { return m_Data.Title; }
		GLFWwindow* GetNativeWindow() override { return m_Window; }

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

		WindowConfig m_Config;
		GLFWwindow* m_Window;
		GLFWmonitor* m_Monitor;

		glm::ivec2 monitorSize;
		glm::ivec2 monitorPos;
		glm::ivec2 windowSize;
		glm::ivec2 windowPos;
	};
}



