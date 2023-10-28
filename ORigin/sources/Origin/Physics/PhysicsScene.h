#pragma once

#include "Origin\Scene\Scene.h"

namespace origin {

	class PhysicsScene
	{
	public:
		PhysicsScene() = default;
		PhysicsScene(const std::shared_ptr<Scene>& scene);
		
	};
}

