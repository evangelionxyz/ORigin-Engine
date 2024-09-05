// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "OpenGLContext.h"
#include "Origin/Profiler/Profiler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace origin
{
	void OpenGLContext::Init(Window *window)
	{
		OGN_PROFILER_RENDERING();

		const i32 success = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
		OGN_CORE_ASSERT(success, "Failed to initialize Glad!");

		OGN_CORE_TRACE("OpenGL Info");
		OGN_CORE_TRACE(" Version : {}", reinterpret_cast<const char *>(glGetString(GL_VERSION)));
		OGN_CORE_TRACE(" Vendor  : {}", reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
		OGN_CORE_TRACE(" Renderer: {}", reinterpret_cast<const char *>(glGetString(GL_RENDERER)));

		const i32 major = GLVersion.major;
		const i32 minor = GLVersion.minor;
		OGN_CORE_ASSERT(major == 4 && minor >= 5, "ORigin requires at least OpenGL version 4.5!");
	}

	void OpenGLContext::Shutdown()
	{
		
	}
}