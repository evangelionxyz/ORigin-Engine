// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

namespace Origin
{
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		static std::unique_ptr<GraphicsContext> Create(void* window);
	};
}