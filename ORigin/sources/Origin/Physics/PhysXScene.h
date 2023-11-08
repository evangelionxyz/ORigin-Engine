#pragma once

#include "PhysicsScene.h"
#include "PxPhysicsAPI.h"

namespace origin {

	class RigidbodyComponent;

	class PhysXScene : public PhysicsScene
	{
	public:
		PhysXScene(Scene* scene);
		~PhysXScene();

		virtual void OnSimulationStart() override;
		virtual void OnSimulationStop() override;

		virtual void Simulate(float deltaTime) override;
		virtual physx::PxRigidActor* CreateActor(RigidbodyComponent& rb, const glm::vec3& position, const glm::quat& rot) override;

		virtual physx::PxScene* GetScene() override { return m_PhysXScene; }

	private:
		physx::PxTolerancesScale m_ToleranceScale;
		physx::PxScene* m_PhysXScene = nullptr;
		physx::PxMaterial* m_Material = nullptr;

		Scene* m_Context = nullptr;
	};

}

