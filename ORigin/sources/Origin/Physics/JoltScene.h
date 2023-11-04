#pragma once

#include "Jolt\Jolt.h"
#include "Jolt\Physics\PhysicsSystem.h"
#include "Jolt\Physics\Collision\ContactListener.h"
#include "Jolt\Physics\PhysicsSettings.h"

namespace JPH {
	class JobSystem;
	class TempAllocator;
}

namespace origin {

	class JoltScene
	{
	public:
		JoltScene();

	private:
		int m_MaxConcurrentJobs = std::thread::hardware_concurrency();

		JPH::TempAllocator* m_TempAllocator = nullptr;
		JPH::JobSystem* m_JobSystem = nullptr;
		JPH::JobSystem* m_JobSystemValidating = nullptr;
		JPH::PhysicsSystem* m_PhysicsSystem = nullptr;
		//JPH::ContactListenerImpl* m_ContactListenerImpl = nullptr;
		JPH::PhysicsSettings m_PhysicsSettings;

	};
}

