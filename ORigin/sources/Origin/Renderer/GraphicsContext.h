// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

#include "Origin/Core/Window.h"

namespace origin 
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		virtual void Init(Window *window) = 0;
		virtual void Shutdown() = 0;
		static std::shared_ptr<GraphicsContext> Create();

		template<typename T>
		static std::shared_ptr<T> GetContext() { return std::static_pointer_cast<T>(s_Instance); }

	private:
		static std::shared_ptr<GraphicsContext> s_Instance;
	};
}

#endif