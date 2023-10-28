#pragma once

#include "PxPhysicsAPI.h"

#include "PhysicsAPI.h"

namespace origin {

	

	class PhysXAPI : public PhysicsAPI
	{
	public:
		virtual void Init() override;
		virtual void Shutdown() override;
		virtual std::shared_ptr<PhysicsScene> CreateScene(const std::shared_ptr<Scene>& scene) const override;
		virtual const std::string& GetLastErrorMessage() const override;

		static physx::PxPhysics* GetPhysics();
		static physx::PxDefaultCpuDispatcher* GetCPUDispatcher();
	};
}

