// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <GLFW/glfw3.h>
#include "Origin/IO/Events/Event.h"

namespace Origin
{
	class Window
	{
	public:
		virtual ~Window() = default;
		virtual void SetEventCallback(const std::function<void(Event&)>& callback) = 0;
		virtual void OnUpdate() = 0;
		virtual bool Loop() const = 0;
		virtual void SetFullscreen(bool enable) = 0;
		virtual void SetClose(bool close) = 0;
		virtual void SetVSync(bool enable = false) = 0;

		virtual void SetIcon(const std::string& filepath) = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual std::string GetTitle() const = 0;

		virtual GLFWwindow* GetMainWindow() = 0;

		static std::unique_ptr<Window> Create(const std::string& title);
	};
}