// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

namespace origin {
	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		virtual void Init() = 0;
		static std::unique_ptr<GraphicsContext> Create();
	};
}