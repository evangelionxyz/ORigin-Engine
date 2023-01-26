// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "RenderCommand.h"

namespace Origin
{
	class Renderer
	{
	public:
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnUpdate();
		void DrawLineMode(bool enable);
	};
}