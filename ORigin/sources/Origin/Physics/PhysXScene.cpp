#include "pch.h"
#include "PhysXScene.h"
#include "PhysXAPI.h"
#include "PxActor.h"

#include "Origin\Scene\Entity.h"

namespace origin {

	PhysXScene::PhysXScene(Scene* scene)
		: m_Context(scene)
	{
		physx::PxSceneDesc sceneDesc(PhysXAPI::GetPhysics()->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.cpuDispatcher = PhysXAPI::GetCPUDispatcher();
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

		m_PhysXScene = PhysXAPI::GetPhysics()->createScene(sceneDesc);
	}

	PhysXScene::~PhysXScene()
	{
		m_PhysXScene->release();
		m_PhysXScene = nullptr;

		m_Context = nullptr;
	}

	void PhysXScene::OnSimulationStart()
	{
		for (auto& id : m_Context->m_Registry.view<BoxColliderComponent>())
		{
			Entity entity = { id, m_Context };
			if (!entity.GetComponent<BoxColliderComponent>().Rigidbody)
				entity.GetComponent<BoxColliderComponent>().Create(entity, this);
		}

		for (auto& id : m_Context->m_Registry.view<SphereColliderComponent>())
		{
			Entity entity = { id, m_Context };
			if (!entity.GetComponent<SphereColliderComponent>().Rigidbody)
				entity.GetComponent<SphereColliderComponent>().Create(entity, this);
		}
	}

	void PhysXScene::OnSimulationStop()
	{
		for (auto& id : m_Context->m_Registry.view<BoxColliderComponent>())
		{
			Entity entity = { id, m_Context };
			entity.GetComponent<BoxColliderComponent>().Destroy();
		}

		for (auto& id : m_Context->m_Registry.view<SphereColliderComponent>())
		{
			Entity entity = { id, m_Context };
			entity.GetComponent<SphereColliderComponent>().Destroy();
		}

		for (auto& id : m_Context->m_Registry.view<RigidbodyComponent>())
		{
			Entity entity = { id, m_Context };
			auto& body = entity.GetComponent<RigidbodyComponent>().Body;

			if (body)
			{
				m_PhysXScene->removeActor(*((physx::PxActor*)body), false);
				((physx::PxActor*)body)->isReleasable() ? ((physx::PxActor*)body)->release() : 0;
				body = nullptr;
			}
		}
	}

	void PhysXScene::Simulate(float deltaTime)
	{
		m_PhysXScene->simulate(deltaTime);
		m_PhysXScene->fetchResults(true);

		auto view = m_Context->m_Registry.view<RigidbodyComponent>();
		for (auto id : view)
		{
			Entity entity = { id, m_Context };

			auto& rb = entity.GetComponent<RigidbodyComponent>();
			auto& tc = entity.GetComponent<TransformComponent>();

			if (rb.Body == nullptr)
				continue;

			physx::PxRigidActor* actor = (physx::PxRigidActor*)rb.Body;
			physx::PxTransform tr = actor->getGlobalPose();
			glm::vec3 pos = Utils::FromPhysXVec3(tr.p);
			glm::quat rot = glm::quat(tr.q.w, tr.q.x, tr.q.y, tr.q.z);

			tc.Translation = pos;
			tc.Rotation = glm::eulerAngles(rot);
		}
	}

	physx::PxRigidActor* PhysXScene::CreateActor(RigidbodyComponent& rb, const glm::vec3& position, const glm::quat& rot)
	{
		physx::PxPhysics* physics = PhysXAPI::GetPhysics();

		physx::PxTransform transform = physx::PxTransform(Utils::ToPhysXVec3(position), Utils::ToPhysXQuat(rot));

		physx::PxRigidActor* actor;
		actor = physics->createRigidDynamic(transform);

		OGN_CORE_ASSERT(actor);
		actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, !rb.UseGravity);

		physx::PxRigidDynamic* dActor = actor->is<physx::PxRigidDynamic>();
		OGN_CORE_ASSERT(dActor);

		dActor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, rb.Kinematic);

		if (rb.Kinematic)
			dActor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eRETAIN_ACCELERATIONS, rb.RetainAcceleration);

		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, !rb.RotateX);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, !rb.RotateY);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, !rb.RotateZ);

		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, !rb.MoveX);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, !rb.MoveY);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, !rb.MoveZ);

		physx::PxRigidBodyExt::updateMassAndInertia(*dActor, rb.Mass, &Utils::ToPhysXVec3(rb.CenterMassPosition));
		rb.Body = (void*)actor;

		return actor;
	}

}
