// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Win_Window.h"
#include "origin\Renderer\Icon.h"

#include "Origin\IO\KeyCodes.h"
#include "Origin\IO\Events\KeyEvent.h"
#include "Origin\IO\Events\MouseEvent.h"
#include "Origin\IO\Events\AppEvent.h"
#include "Origin\Core\Application.h"

namespace origin {

	WinWindow::WinWindow(const WindowConfig& config) : m_Config(config)
	{
		m_Data.Title = config.Title;
		m_Data.Width = config.Width;
		m_Data.Height = config.Height;
		Init();
	}

	WinWindow::WinWindow(const std::string& title, uint32_t width, uint32_t height)
	{
		m_Data.Width = width;
		m_Data.Height = height;

		m_Data.Title = title;
		Init();
	}

	WinWindow::~WinWindow()
	{
		Destroy();
	}

	void WinWindow::Init()
	{
		m_Monitor = glfwGetPrimaryMonitor();
		glfwGetMonitorWorkarea(m_Monitor, &monitorPos.x, &monitorPos.y, &monitorSize.x, &monitorSize.y);

		windowSize.x = (int)m_Data.Width;
		windowSize.y = (int)m_Data.Height;

#if defined(OGN_DEBUG)
		if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
		{
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		}
#endif
		glfwWindowHint(GLFW_DECORATED, m_Config.Decorated ? GLFW_TRUE : GLFW_FALSE);
		m_Window = glfwCreateWindow(windowSize.x, windowSize.y, m_Data.Title.c_str(), nullptr, nullptr);

		if (!m_Window)
		{
			OGN_CORE_ERROR("ERROR : FAILED TO CREATE WINDOW");
			glfwTerminate();
		}

		m_Data.xPos = (monitorSize.x - windowSize.x) / 2;
		m_Data.yPos = (monitorSize.y - windowSize.y) / 2;
		windowPos.x = m_Data.xPos;
		windowPos.y = m_Data.yPos;
		glfwSetWindowPos(m_Window, m_Data.xPos, m_Data.yPos);
	}

	void WinWindow::SetEventCallback(const std::function<void(Event&)>& callback)
	{
		m_Data.EventCallback = callback;
		WindowCallbacks();
	}

	void WinWindow::OnUpdate()
	{
		glfwPollEvents();
		if (m_Data.Fullscreen)
			glfwSetWindowMonitor(m_Window, m_Monitor, monitorPos.x, monitorPos.y, monitorSize.x, monitorSize.y, 0);

		if(!m_Data.Fullscreen && !m_Data.Maximized)
		{
			if ((int)m_Data.Width >= monitorSize.x)
				m_Data.Width = windowSize.x;
			if ((int)m_Data.Height >= monitorSize.y)
				m_Data.Height = windowSize.y;

			if ((int)m_Data.xPos == 0)
				m_Data.xPos = windowPos.x;
			if ((int)m_Data.yPos == 0)
				m_Data.yPos = windowPos.y;

			glfwSetWindowMonitor(m_Window, nullptr, (int)m_Data.xPos, (int)m_Data.yPos, (int)m_Data.Width, (int)m_Data.Height, 0);
		}

		auto& app = Application::Get();

		if (!app.GetMinimized())
			glfwSwapBuffers(m_Window);
	}

	void WinWindow::SetVSync(bool enable)
	{
		if (enable)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enable;
	}

	void WinWindow::SetClose(bool close)
	{
		if (close)
			m_Data.Close = 1;
		else
			m_Data.Close = 0;
	}

	// Set Window Callbacks
	void WinWindow::WindowCallbacks()
	{
		glfwSetWindowUserPointer(m_Window, &m_Data);

		glfwSetWindowMaximizeCallback(m_Window, [](GLFWwindow* window, int maximized)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Maximized = (bool)maximized;
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xpos, int ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.xPos = xpos;
				data.yPos = ypos;
			});

		glfwSetMonitorCallback([](GLFWmonitor* monitor, int event)
			{

			});

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallback(event);
						break;
					}

					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallback(event);
						break;
					}

					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.EventCallback(event);
						break;
					}
				}
			});

		

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});

		glfwSetDropCallback(m_Window, [](GLFWwindow* window, int pathCount, const char* paths[])
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				std::vector<std::filesystem::path> filepaths(pathCount);
				for (int i = 0; i < pathCount; i++)
					filepaths[i] = paths[i];

				WindowDropEvent event(std::move(filepaths));
				data.EventCallback(event);
			});
	}

	void WinWindow::Destroy()
	{
		glfwMakeContextCurrent(nullptr);
		glfwDestroyWindow(m_Window);
	}

	void WinWindow::Decorated(bool enable)
	{
		m_Config.Decorated = enable;
		glfwSetWindowAttrib(m_Window, GLFW_DECORATED, enable ? GLFW_TRUE : GLFW_FALSE);
	}

	void WinWindow::SetSize(uint32_t width, uint32_t height)
	{
		m_Data.Width = width;
		m_Data.Height = height;

		glfwSetWindowSize(m_Window, width, height);

		m_Data.xPos = (monitorSize.x - width) / 2;
		m_Data.yPos = (monitorSize.y - height) / 2;

		windowPos.x = m_Data.xPos;
		windowPos.y = m_Data.yPos;
		glfwSetWindowPos(m_Window, m_Data.xPos, m_Data.yPos);
	}

	void WinWindow::SetIcon(const std::string& filepath)
	{
		Icon icon(m_Window, filepath);
	}
}
