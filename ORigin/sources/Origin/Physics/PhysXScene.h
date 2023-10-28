#pragma once

#include "PhysicsScene.h"

#include "PxPhysicsAPI.h"

namespace origin {

	class PhysXScene : public PhysicsScene
	{
	public:
		PhysXScene() = default;
		PhysXScene(const std::shared_ptr<Scene>& scene);

		void Simulate(float deltaTime);

		static std::shared_ptr<PhysXScene> Create(const std::shared_ptr<Scene>& scene);

	private:
		physx::PxTolerancesScale m_ToleranceScale;
		physx::PxScene* m_Scene = nullptr;
		physx::PxMaterial* m_Material = nullptr;
		physx::PxPvd* m_Pvd = nullptr;
	};

}

