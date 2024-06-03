// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\GraphicsContext.h"

namespace origin
{
	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext();
		void Init(Window *window) override;
	};
}