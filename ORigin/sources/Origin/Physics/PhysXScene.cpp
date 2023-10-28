#include "pch.h"
#include "PhysXScene.h"

#include "PhysXAPI.h"

namespace origin {

	PhysXScene::PhysXScene(const std::shared_ptr<Scene>& scene)
	{
		physx::PxSceneDesc sceneDesc(PhysXAPI::GetPhysics()->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.cpuDispatcher = PhysXAPI::GetCPUDispatcher();
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

		m_Scene = PhysXAPI::GetPhysics()->createScene(sceneDesc);

		//m_Pvd = m_Scene->getScenePvdClient();
	}

	void PhysXScene::Simulate(float deltaTime)
	{
		m_Material = PhysXAPI::GetPhysics()->createMaterial(0.5f, 0.5f, 0.6f);


	}

	std::shared_ptr<PhysXScene> PhysXScene::Create(const std::shared_ptr<Scene>& scene)
	{
		return std::make_shared<PhysXScene>(scene);
	}

}
