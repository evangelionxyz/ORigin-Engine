#pragma once
#include "Scene.h"

namespace origin
{
	class SceneRenderer
	{
	public:

	private:
		void RenderPass();
		void LightingPass();
		void ShadowPass();

		Scene *scene = nullptr;
	};
}

