// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGLContext.h"
#include <glad/glad.h>

#include <GLFW/glfw3.h>

namespace origin
{
	OpenGLContext::OpenGLContext()
	{
	}

	void OpenGLContext::Init(Window *window)
	{
		OGN_PROFILER_RENDERING();

		int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		OGN_CORE_ASSERT(success, "Failed to initialize Glad!");

		OGN_CORE_TRACE("OpenGL Info");
		OGN_CORE_TRACE(" Version : {0}", glGetString(GL_VERSION));
		OGN_CORE_TRACE(" Vendor  : {0}", glGetString(GL_VENDOR));
		OGN_CORE_TRACE(" Renderer: {0}", glGetString(GL_RENDERER));

		OGN_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 5),
			"ORigin requires at least OpenGL version 4.5!");
	}
}