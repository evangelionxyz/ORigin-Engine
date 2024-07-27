#pragma once
#include "Origin/Core/Time.h"
#include "Origin/Scene/Entity.h"
#include <glm/glm.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyInterface.h>

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

		static void OnInstantiateScriptEntity(Entity entity);
		static void OnDestroyScriptEntity(Entity entity);

		static JPH::BodyInterface *GetBodyInterface();
	};
}

