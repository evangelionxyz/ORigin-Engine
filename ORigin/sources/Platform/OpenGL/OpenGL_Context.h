// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\GraphicsContext.h"
#include <GLFW\glfw3.h>

namespace Origin
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* windowHandle);

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}