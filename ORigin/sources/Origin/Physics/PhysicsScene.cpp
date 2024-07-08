// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "PhysicsScene.h"
#include "PhysicsAPI.h"

namespace origin {

	std::unique_ptr<PhysicsScene> PhysicsScene::Create(Scene* scene)
	{
		OGN_PROFILER_PHYSICS();

		switch (PhysicsAPI::Current())
		{
		default:
		case PhysicsAPIType::Jolt: break;
		case PhysicsAPIType::None: break;
		}

		OGN_CORE_ASSERT(false, "PhysicScene : Invalid Physics API");
		return nullptr;
	}
}
