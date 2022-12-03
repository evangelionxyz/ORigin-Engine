// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGL_Context.h"
#include <glad/glad.h>

namespace Origin
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		OGN_CORE_ASSERT(windowHandle, "Window Handle is null");
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		OGN_CORE_ASSERT(success, "Failed to initialize Glad!");

		OGN_CORE_INFO("OpenGL Info");
		OGN_CORE_INFO(" Version : {0}", glGetString(GL_VERSION));
		OGN_CORE_INFO(" Vendor  : {0}", glGetString(GL_VENDOR));
		OGN_CORE_INFO(" Renderer: {0}", glGetString(GL_RENDERER));

		OGN_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5),
			"ORigin requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}