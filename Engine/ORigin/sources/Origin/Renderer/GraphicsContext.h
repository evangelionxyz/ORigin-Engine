// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

namespace origin
{
	class SDLWindow;

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;
		virtual void Init(SDLWindow *window) = 0;
		virtual void Shutdown() = 0;
		static Ref<GraphicsContext> Create();

		template<typename T>
        static Ref<T> GetContext() { return std::static_pointer_cast<T>(s_Instance); }

	private:
		static Ref<GraphicsContext> s_Instance;
	};
}

#endif