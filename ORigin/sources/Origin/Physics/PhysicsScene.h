// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Scene/Scene.h"

namespace origin {

	class PhysicsScene
	{
	public:
		PhysicsScene() {}
		~PhysicsScene() {}
		virtual void OnSimulationStart() = 0;
		virtual void OnSimulationStop() = 0;
		virtual void Simulate(float deltaTime) = 0;
		static std::unique_ptr<PhysicsScene> Create(Scene* scene);
	};
}

