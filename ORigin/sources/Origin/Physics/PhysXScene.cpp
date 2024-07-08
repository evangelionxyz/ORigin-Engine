// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ColliderComponent.h"
#include "RigidbodyComponent.h"
#include "Origin/Scene/Entity.h"

#include "PhysXScene.h"
#include "PhysXAPI.h"
#include "PxActor.h"

namespace origin {

	PhysXScene::PhysXScene(Scene* scene)
		: m_Scene(scene)
	{
	}

	PhysXScene::~PhysXScene()
	{
		m_Scene = nullptr;
	}

	void PhysXScene::OnSimulationStart()
	{
		OGN_PROFILER_PHYSICS();

		PhysXAPI::Get().OnSimulationStart();

		physx::PxSceneDesc sceneDesc(PhysXAPI::GetPhysics()->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		sceneDesc.cpuDispatcher = PhysXAPI::GetCPUDispatcher();
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		m_PhysXScene = PhysXAPI::GetPhysics()->createScene(sceneDesc);


		for (auto& id : m_Scene->m_Registry.view<BoxColliderComponent>())
		{
			OGN_PROFILER_SCOPE("Creating BoxColliderComponent");

			Entity entity = { id, m_Scene };
			if (entity.IsValid())
			{
				if (!entity.GetComponent<BoxColliderComponent>().Rigidbody)
					entity.GetComponent<BoxColliderComponent>().Create(entity, this);
			}
		}

		for (auto &id : m_Scene->m_Registry.view<SphereColliderComponent>())
		{
			Entity entity = { id, m_Scene };
			if (entity.IsValid())
			{
				if (!entity.GetComponent<SphereColliderComponent>().Rigidbody)
					entity.GetComponent<SphereColliderComponent>().Create(entity, this);
			}
		}

		for (auto& id : m_Scene->m_Registry.view<CapsuleColliderComponent>())
		{
			Entity entity = { id, m_Scene };
			if (entity.IsValid())
			{
				if (!entity.GetComponent<CapsuleColliderComponent>().Rigidbody)
					entity.GetComponent<CapsuleColliderComponent>().Create(entity, this);
			}
		}
	}

	void PhysXScene::OnSimulationStop()
	{
		OGN_PROFILER_PHYSICS();

		for (auto& id : m_Scene->m_Registry.view<BoxColliderComponent>())
		{
			Entity entity = { id, m_Scene };
			if (entity.IsValid())
			{
				entity.GetComponent<BoxColliderComponent>().Destroy();
			}
		}

		for (auto &id : m_Scene->m_Registry.view<SphereColliderComponent>())
		{
			Entity entity = { id, m_Scene };
			if (entity.IsValid())
			{
				entity.GetComponent<SphereColliderComponent>().Destroy();
			}
		}

		for (auto& id : m_Scene->m_Registry.view<CapsuleColliderComponent>())
		{
			Entity entity = { id, m_Scene };
			if (entity.IsValid())
			{
				entity.GetComponent<CapsuleColliderComponent>().Destroy();
			}
		}

		for (auto &id : m_Scene->m_Registry.view<RigidbodyComponent>())
		{
			Entity entity = { id, m_Scene };
			if (entity.IsValid())
			{
				auto &body = entity.GetComponent<RigidbodyComponent>().Body;
				if (body)
				{
					physx::PxActor *actor = (physx::PxActor *)body;
					m_PhysXScene->removeActor(*actor, false);
					if (actor->isReleasable())
					{
						actor->release();
					}

					if(body)
						body = nullptr;
				}
			}
		}

		m_PhysXScene->release();
		PhysXAPI::Get().OnSimulationStop();
	}

	void PhysXScene::Simulate(float deltaTime)
	{
		OGN_PROFILER_PHYSICS();

		m_PhysXScene->simulate(deltaTime);
		m_PhysXScene->fetchResults(true);

		auto view = m_Scene->m_Registry.view<RigidbodyComponent>();
		for (auto id : view)
		{
			Entity entity = { id, m_Scene };

			if (entity.IsValid())
			{
				auto &rb = entity.GetComponent<RigidbodyComponent>();
				auto &tc = entity.GetComponent<TransformComponent>();

				if (rb.Body == nullptr)
					continue;

				physx::PxRigidActor *actor = (physx::PxRigidActor *)rb.Body;
				physx::PxTransform tr = actor->getGlobalPose();
				glm::vec3 pos = Utils::FromPhysXVec3(tr.p);
				glm::quat rot = glm::quat(tr.q.w, tr.q.x, tr.q.y, tr.q.z);

				tc.Translation = pos;
				tc.Rotation = glm::eulerAngles(rot);
			}
		}
	}

	physx::PxRigidActor* PhysXScene::CreateActor(RigidbodyComponent& rb, const glm::vec3& position, const glm::quat& rot)
	{
		OGN_PROFILER_PHYSICS();

		physx::PxPhysics* physics = PhysXAPI::GetPhysics();

		physx::PxTransform transform = physx::PxTransform(Utils::ToPhysXVec3(position), Utils::ToPhysXQuat(rot));

		physx::PxRigidActor* actor;
		actor = physics->createRigidDynamic(transform);

		OGN_CORE_ASSERT(actor, "PhysX Invalid RigidActor");
		actor->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_GRAVITY, !rb.UseGravity);

		physx::PxRigidDynamic* dActor = actor->is<physx::PxRigidDynamic>();
		OGN_CORE_ASSERT(dActor, "PhysX Invalid RigidDynamicActor");

		dActor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, rb.Kinematic);

		if (rb.Kinematic)
			dActor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eRETAIN_ACCELERATIONS, rb.RetainAcceleration);

		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, !rb.RotateX);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, !rb.RotateY);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, !rb.RotateZ);

		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, !rb.MoveX);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, !rb.MoveY);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, !rb.MoveZ);

		physx::PxVec3 centerMassLocalPos = Utils::ToPhysXVec3(rb.CenterMassPosition);
		physx::PxRigidBodyExt::updateMassAndInertia(*dActor, rb.Mass, &centerMassLocalPos);
		rb.Body = (void*)actor;

		return actor;
	}

}
