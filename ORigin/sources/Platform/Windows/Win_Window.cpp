// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Win_Window.h"
#include "Origin\Utils\log.h"
#include "origin\Renderer\Icon.h"

#include "Origin\IO\KeyCodes.h"
#include "Origin\IO\Events\KeyEvent.h"
#include "Origin\IO\Events\MouseEvent.h"
#include "Origin\IO\Events\AppEvent.h"
#include "Origin\Core\Application.h"

namespace Origin
{
	WinWindow::WinWindow(const std::string& title)
	{
		m_Data.Width = 1280;
		m_Data.Height = 640;
		m_Data.Title = title;
		Init();
	}

	WinWindow::~WinWindow()
	{
		glfwTerminate();
		glfwDestroyWindow(m_Window);
	}

	void WinWindow::Init()
	{
		if (!glfwInit())
			OGN_CORE_ASSERT(false, "ERROR :FAILED TO INITIALIZE GLFW");

		m_Monitor = glfwGetPrimaryMonitor();
		glfwGetMonitorWorkarea(m_Monitor, &monitorPos.x, &monitorPos.y, &monitorSize.x, &monitorSize.y);

		windowSize.x = (int)m_Data.Width;
		windowSize.y = (int)m_Data.Height;

#if defined(OGN_DEBUG)
		if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		m_Window = glfwCreateWindow(windowSize.x, windowSize.y, m_Data.Title.c_str(), nullptr, nullptr);

		if (!m_Window) {
			OGN_CORE_ERROR("ERROR : FAILED TO CREATE WINDOW");
			glfwTerminate();
		}

		m_Data.xPos = (monitorSize.x - windowSize.x) / 2;
		m_Data.yPos = (monitorSize.y - windowSize.y) / 2;
		windowPos.x = m_Data.xPos;
		windowPos.y = m_Data.yPos;
		glfwSetWindowPos(m_Window, m_Data.xPos, m_Data.yPos);

		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		WindowCallbacks();
	}

	void WinWindow::OnUpdate()
	{
		glfwPollEvents();
		if (m_Data.Fullscreen)
		{
			glfwSetWindowMonitor(m_Window, m_Monitor, monitorPos.x, monitorPos.y, monitorSize.x, monitorSize.y, 0);
		}

		if (!m_Data.Fullscreen)
		{
			if (!m_Data.Maximized)
			{
				if ((int)m_Data.Width >= monitorSize.x)
					m_Data.Width = windowSize.x;
				if ((int)m_Data.Height >= monitorSize.y)
					m_Data.Height = windowSize.y;

				if ((int)m_Data.xPos == 0)
					m_Data.xPos = windowPos.x;
				if ((int)m_Data.yPos == 0)
					m_Data.yPos = windowPos.y;
			}
			glfwSetWindowMonitor(m_Window, nullptr, (int)m_Data.xPos, (int)m_Data.yPos, (int)m_Data.Width, (int)m_Data.Height, 0);
		}

		auto& app = Application::Get();
		if (!app.GetMinimized())
			m_Context->SwapBuffers();
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
	}

	void WinWindow::SetIcon(const std::string& filepath)
	{
		Icon icon(m_Window, filepath);
	}
}
