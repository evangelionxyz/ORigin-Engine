#pragma once

#include "Origin/Core/Time.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Scene/Components/PhysicsComponents.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <glm/glm.hpp>

namespace origin {

	class Scene;
	class PhysicsEngine
	{
	public:
		static void Init();
		static void Shutdown();

        static void CreateBoxCollider(Entity entity);
        static void CreateSphereCollider(Entity entity);

		static void OnSimulateStart(Scene *scene);
		static void OnSimulateStop(Scene *scene);
		static void Simulate(Timestep ts, Scene *scene);
	};
}

