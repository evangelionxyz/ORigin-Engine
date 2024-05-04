// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Win_Window.h"

#include "Origin\Core\KeyCodes.h"
#include "Origin\Core\KeyEvent.h"
#include "Origin\Core\MouseEvent.h"
#include "Origin\Core\AppEvent.h"
#include "Origin\Core\Application.h"
#include "Origin\Asset\AssetImporter.h"

#include "stb_image.h"

#include <glad\glad.h>

namespace origin {

	static int initWinPosX = 0;
	static int initWinPosY = 0;

	WinWindow::WinWindow(const char* title, uint32_t width, uint32_t height, bool maximized)
	{
		OGN_PROFILER_FUNCTION();

#if defined(OGN_DEBUG)
		if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		glfwWindowHint(GLFW_MAXIMIZED, (int)maximized);
		m_MainWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
		

		if (!m_MainWindow)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		if (!maximized)
		{
			int mSizeX, mSizeY;
			glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), nullptr, nullptr, &mSizeX, &mSizeY);
			m_Data.xPos = (mSizeX - width) / 2;
			m_Data.yPos = (mSizeY - height) / 2;
			glfwSetWindowPos(m_MainWindow, m_Data.xPos, m_Data.yPos);
		}

		glfwMakeContextCurrent(m_MainWindow);

		m_GraphicsContext = GraphicsContext::Create();
		m_GraphicsContext->Init();

		int w, h;
		glfwGetWindowSize(m_MainWindow, &w, &h);
		m_Data.Width = (uint32_t)w;
		m_Data.Height = (uint32_t)h;

		glfwGetWindowPos(m_MainWindow, &initWinPosX, &initWinPosY);
	}

	WinWindow::~WinWindow()
	{
		glfwDestroyWindow(m_MainWindow);
	}

	void WinWindow::SetEventCallback(const std::function<void(Event&)>& callback)
	{
		OGN_PROFILER_FUNCTION();

		m_Data.EventCallback = callback;
		WindowCallbacks();
	}

	void WinWindow::OnUpdate()
	{
		OGN_PROFILER_FUNCTION();

		glfwPollEvents();
		glfwSwapBuffers(m_MainWindow);
	}

	void WinWindow::SetVSync(bool enable)
	{
		OGN_PROFILER_FUNCTION();

		glfwSwapInterval(enable ? 1 : 0);
		m_Data.VSync = enable;
	}

	void WinWindow::SetClose(bool close)
	{
		m_Data.Close = close ? 1 : 0;
	}

	void WinWindow::SetFullscreen(bool enable)
	{
		OGN_PROFILER_FUNCTION();

		if (enable)
		{
			int width, height;
			glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), nullptr, nullptr, &width, &height);
			glfwSetWindowMonitor(m_MainWindow, glfwGetPrimaryMonitor(), 0, 0, width, height, GLFW_DONT_CARE);
		}
		else
		{
			glfwSetWindowMonitor(m_MainWindow, nullptr, initWinPosX, initWinPosY, m_Data.Width, m_Data.Height, GLFW_DONT_CARE);
		}
	}


	void WinWindow::SetIcon(const char* filepath)
	{
		OGN_PROFILER_FUNCTION();

		stbi_set_flip_vertically_on_load(0);
		int width, height, bpp;
		unsigned char* data = stbi_load(filepath, &width, &height, &bpp, 0);

		GLFWimage icon[1];
		icon[0].width = width;
		icon[0].height = height;
		icon[0].pixels = data;
		glfwSetWindowIcon(m_MainWindow, 1, icon);
		stbi_image_free(data);
	}

	void WinWindow::WindowCallbacks()
	{
		OGN_PROFILER_FUNCTION();

		glfwSetWindowUserPointer(m_MainWindow, &m_Data);

		glfwSetWindowMaximizeCallback(m_MainWindow, [](GLFWwindow* window, int maximized)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Maximized = (bool)maximized;
			});

		glfwSetWindowPosCallback(m_MainWindow, [](GLFWwindow* window, int xpos, int ypos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.xPos = xpos;
				data.yPos = ypos;
			});

		glfwSetFramebufferSizeCallback(m_MainWindow, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				FramebufferResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowSizeCallback(m_MainWindow, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_MainWindow, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetKeyCallback(m_MainWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
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

		glfwSetCharCallback(m_MainWindow, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(m_MainWindow, [](GLFWwindow* window, int button, int action, int mods)
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

		glfwSetScrollCallback(m_MainWindow, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(m_MainWindow, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});

		glfwSetDropCallback(m_MainWindow, [](GLFWwindow* window, int pathCount, const char* paths[])
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				std::vector<std::filesystem::path> filepaths(pathCount);
				for (int i = 0; i < pathCount; i++)
					filepaths[i] = paths[i];

				WindowDropEvent event(std::move(filepaths));
				data.EventCallback(event);
			});
	}

	
}
