// Copyright (c) Evangelion Manuhutu | ORigin Engine



#include "Physics.h"
#include "PhysXAPI.h"

namespace origin {

	std::unique_ptr<PhysicsAPI> Physics::m_PhysicsAPI;

	void Physics::Init()
	{
		OGN_PROFILER_PHYSICS();

		switch (PhysicsAPI::Current())
		{
		case PhysicsAPIType::Jolt: break;
		case PhysicsAPIType::PhysX: 
			m_PhysicsAPI = std::make_unique<PhysXAPI>();
			m_PhysicsAPI->Init();
			break;
		case PhysicsAPIType::None: break;
		}
	}

	void Physics::Shutdown()
	{
		OGN_PROFILER_PHYSICS();

		if (!m_PhysicsAPI)
			return;

		switch (PhysicsAPI::Current())
		{
		case PhysicsAPIType::Jolt: break;
		case PhysicsAPIType::PhysX:
			m_PhysicsAPI->Shutdown();
			break;
		case PhysicsAPIType::None: break;
		}
	}
}