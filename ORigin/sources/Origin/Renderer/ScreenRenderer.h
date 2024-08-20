// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SCREEN_RENDERER_H
#define SCREEN_RENDERER_H

#include "Framebuffer.h"

namespace origin
{
	class ScreenRenderer
	{ 
	public:
		ScreenRenderer() = default;

		virtual void Resize(float width, float height) = 0;
		virtual void Render() = 0;

		static std::shared_ptr<ScreenRenderer> Create(std::shared_ptr<Framebuffer> Framebuffer, float width, float height);
	private:
	};
}

#endif