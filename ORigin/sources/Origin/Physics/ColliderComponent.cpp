#include "pch.h"
#include "ColliderComponent.h"

#include "PhysXAPI.h"

#include "PxRigidActor.h"

#include "Origin\Scene\Entity.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

namespace origin {

	void BoxColliderComponent::Create(Entity entity, PhysicsScene* physxScene)
	{
		PROFILER_PHYSICS();

		auto& tc = entity.GetComponent<TransformComponent>();
		auto& bc = entity.GetComponent<BoxColliderComponent>();

		physx::PxRigidActor* actor;
		if (entity.HasComponent<RigidbodyComponent>())
		{
			RigidbodyComponent& rb = entity.GetComponent<RigidbodyComponent>();
			if (rb.Body)
				actor = (physx::PxRigidBody*)rb.Body;
			else
				actor = physxScene->CreateActor(rb, tc.Translation, glm::quat(tc.Rotation));
		}
		else
		{
			RigidbodyComponent& rb = entity.AddComponent<RigidbodyComponent>();
			rb.UseGravity = true;
			rb.Kinematic = true;
			actor = physxScene->CreateActor(rb, tc.Translation, glm::quat(tc.Rotation));
		}

		RigidbodyComponent& rb = entity.GetComponent<RigidbodyComponent>();
		physx::PxTransform relativePos(Utils::ToPhysXVec3(bc.Offset));
		physx::PxMaterial* pxMaterial = PhysXAPI::GetPhysics()->createMaterial(bc.StaticFriction, bc.DynamicFriction, bc.Restitution);

		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxBoxGeometry(Utils::ToPhysXVec3(tc.Scale * (bc.Size * 2.0f))), *pxMaterial);

		shape->setLocalPose(relativePos);
		bc.Shape = (void*)shape;
		bc.Rigidbody = (void*)actor;

		rb.Body = actor;

		physxScene->GetScene()->addActor(*actor);
	}

	void BoxColliderComponent::Destroy()
	{
		PROFILER_PHYSICS();

		physx::PxShape* shape = (physx::PxShape*)this->Shape;
		physx::PxRigidActor* actor = (physx::PxRigidActor*)Rigidbody;
		actor->detachShape(*shape);
		Shape = nullptr;
		Rigidbody = nullptr;
	}

	void SphereColliderComponent::Create(Entity entity, PhysicsScene* physxScene)
	{
		PROFILER_PHYSICS();

		auto& tc = entity.GetComponent<TransformComponent>();
		auto& sc = entity.GetComponent<SphereColliderComponent>();

		physx::PxRigidActor* actor;
		if (entity.HasComponent<RigidbodyComponent>())
		{
			RigidbodyComponent& rb = entity.GetComponent<RigidbodyComponent>();
			if (rb.Body)
				actor = (physx::PxRigidBody*)rb.Body;
			else
				actor = physxScene->CreateActor(rb, tc.Translation, glm::quat(tc.Rotation));
		}
		else
		{
			RigidbodyComponent& rb = entity.AddComponent<RigidbodyComponent>();
			rb.UseGravity = true;
			rb.Kinematic = true;
			actor = physxScene->CreateActor(rb, tc.Translation, glm::quat(tc.Rotation));
		}

		RigidbodyComponent& rb = entity.GetComponent<RigidbodyComponent>();
		physx::PxTransform relativePos(Utils::ToPhysXVec3(sc.Offset));
		physx::PxMaterial* pxMaterial = PhysXAPI::GetPhysics()->createMaterial(sc.StaticFriction, sc.DynamicFriction, sc.Restitution);

		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxSphereGeometry(tc.Scale.x * sc.Radius * 2.0f), *pxMaterial);

		shape->setLocalPose(relativePos);
		sc.Shape = (void*)shape;
		sc.Rigidbody = (void*)actor;

		rb.Body = actor;

		physxScene->GetScene()->addActor(*actor);

	}

	void SphereColliderComponent::Destroy()
	{
		PROFILER_PHYSICS();

		physx::PxShape* shape = (physx::PxShape*)this->Shape;
		physx::PxRigidActor* actor = (physx::PxRigidActor*)Rigidbody;
		actor->detachShape(*shape);
		Shape = nullptr;
		Rigidbody = nullptr;
	}

	void CapsuleColliderComponent::Create(Entity entity, PhysicsScene* physxScene)
	{
		const auto& tc = entity.GetComponent<TransformComponent>();
		auto& cc = entity.GetComponent<CapsuleColliderComponent>();

		physx::PxRigidActor* actor;
		if (entity.HasComponent<RigidbodyComponent>())
		{
			RigidbodyComponent& rb = entity.GetComponent<RigidbodyComponent>();
			if (rb.Body)
				actor = (physx::PxRigidBody*)rb.Body;
			else
				actor = physxScene->CreateActor(rb, tc.Translation, glm::quat(tc.Rotation));
		}
		else
		{
			RigidbodyComponent& rb = entity.AddComponent<RigidbodyComponent>();
			rb.UseGravity = true;
			rb.Kinematic = true;
			actor = physxScene->CreateActor(rb, tc.Translation, glm::quat(tc.Rotation));
		}

		RigidbodyComponent& rb = entity.GetComponent<RigidbodyComponent>();
		physx::PxTransform relativePos(Utils::ToPhysXVec3(cc.Offset));

		physx::PxMaterial* pxMaterial = PhysXAPI::GetPhysics()->createMaterial(cc.StaticFriction, cc.DynamicFriction, cc.Restitution);

		// this is a horizontal capsule
		m_Geometry = physx::PxCapsuleGeometry(cc.Radius, Height * tc.Scale.x);
		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, m_Geometry, *pxMaterial);

		shape->setLocalPose(relativePos);
		cc.Shape = (void*)shape;
		cc.Rigidbody = (void*)actor;
		rb.Body = actor;
		physxScene->GetScene()->addActor(*actor);
	}

	physx::PxCapsuleGeometry CapsuleColliderComponent::GetGeometry() const
	{
		return m_Geometry;
	}

	physx::PxShape* CapsuleColliderComponent::GetShape() const
	{
		if (this->Shape)
			return (physx::PxShape*)this->Shape;

		return nullptr;
	}

	void CapsuleColliderComponent::Destroy()
	{
		PROFILER_PHYSICS();

		physx::PxShape* shape = (physx::PxShape*)this->Shape;
		physx::PxRigidActor* actor = (physx::PxRigidActor*)Rigidbody;
		actor->detachShape(*shape);
		Shape = nullptr;
		Rigidbody = nullptr;
	}

}