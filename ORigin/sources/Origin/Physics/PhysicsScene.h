// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Scene/Scene.h"
#include "PxRigidActor.h"
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
		virtual physx::PxRigidActor *CreateActor(RigidbodyComponent &rb, const glm::vec3 &position, const glm::quat &rot) = 0;
		virtual physx::PxScene *GetScene() = 0;
	};
}

