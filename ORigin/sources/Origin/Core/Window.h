// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <GLFW/glfw3.h>
#include "Origin/IO/Events/Event.h"

#include "glm\glm.hpp"

namespace origin {

	struct WindowConfig
	{
		std::string Title;
		uint32_t Width = 1280, Height = 640;
		bool Decorated = true;
	};

	class Window
	{
	public:
		static void GLFWInit();
		static void GLFWShutdown();

		virtual ~Window() = default;
		virtual void SetEventCallback(const std::function<void(Event&)>& callback) = 0;
		virtual void OnUpdate() = 0;
		virtual bool Loop() const = 0;
		virtual void SetFullscreen(bool enable) = 0;
		virtual void SetClose(bool close) = 0;
		virtual void SetVSync(bool enable = false) = 0;
		virtual void SetIcon(const std::string& filepath) = 0;
		virtual void Destroy() = 0;
		virtual void Decorated(bool enable) = 0;
		virtual void SetSize(uint32_t width, uint32_t height) = 0;

		virtual glm::uvec2 GetPosition() = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual std::string GetTitle() const = 0;

		virtual GLFWwindow* GetNativeWindow() = 0;

		static std::unique_ptr<Window> Create(const std::string& title, uint32_t width = 1280, uint32_t height = 640);
		static std::unique_ptr<Window> Create(const WindowConfig& config);

	};
}