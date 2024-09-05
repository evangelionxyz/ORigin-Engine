// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

#include "Origin/Renderer/GraphicsContext.h"

namespace origin
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext() = default;

		void Init(Window *window) override;
		void Shutdown() override;
	};
}

#endif