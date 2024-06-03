// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin\Core\Window.h"

namespace origin 
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		virtual void Init(Window *window) = 0;
		static std::shared_ptr<GraphicsContext> Create();
	};
}