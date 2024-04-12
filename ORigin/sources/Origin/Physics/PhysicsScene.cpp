#include "pch.h"
#include "PhysicsScene.h"

#include "PhysicsAPI.h"
#include "PhysXScene.h"

namespace origin {

	std::unique_ptr<PhysicsScene> PhysicsScene::Create(Scene* scene)
	{
		PROFILER_PHYSICS();

		switch (PhysicsAPI::Current())
		{
		default:
		case PhysicsAPIType::PhysX: return std::make_unique<PhysXScene>(scene);
			break;
		case PhysicsAPIType::Jolt:
			break;
		case PhysicsAPIType::None:
			break;
		}

		OGN_CORE_ASSERT(false, "PhysicScene : Invalid Physics API");
		return nullptr;
	}
}
