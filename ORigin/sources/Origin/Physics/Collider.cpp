#include "pch.h"
#include "Collider.h"

#include "PhysXAPI.h"

#include "PxRigidActor.h"

#include "Origin\Scene\Entity.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

namespace origin {

	void BoxColliderComponent::Create(Entity entity, PhysicsScene* physxScene)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		auto& boxCollider = entity.GetComponent<BoxColliderComponent>();

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
		physx::PxTransform relativePos(Utils::ToPhysXVec3(boxCollider.Offset));
		physx::PxMaterial* pxMaterial = PhysXAPI::GetPhysics()->createMaterial(boxCollider.StaticFriction, boxCollider.DynamicFriction, boxCollider.Restitution);

		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxBoxGeometry(Utils::ToPhysXVec3(tc.Scale * (boxCollider.Size * 2.0f))), *pxMaterial);

		shape->setLocalPose(relativePos);
		boxCollider.Shape = (void*)shape;
		boxCollider.Rigidbody = (void*)actor;

		rb.Body = actor;

		physxScene->GetScene()->addActor(*actor);
	}

	void BoxColliderComponent::Destroy()
	{
		physx::PxShape* shape = (physx::PxShape*)this->Shape;
		physx::PxRigidActor* actor = (physx::PxRigidActor*)Rigidbody;
		actor->detachShape(*shape);

		Shape = nullptr;
		Rigidbody = nullptr;
	}

	void SphereColliderComponent::Create(Entity entity, PhysicsScene* physxScene)
	{
		auto& tc = entity.GetComponent<TransformComponent>();
		auto& sphereCollider = entity.GetComponent<SphereColliderComponent>();

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
		physx::PxTransform relativePos(Utils::ToPhysXVec3(sphereCollider.Offset));
		physx::PxMaterial* pxMaterial = PhysXAPI::GetPhysics()->createMaterial(sphereCollider.StaticFriction, sphereCollider.DynamicFriction, sphereCollider.Restitution);

		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxSphereGeometry(tc.Scale.x * (sphereCollider.Radius * 2.0f)), *pxMaterial);

		shape->setLocalPose(relativePos);
		sphereCollider.Shape = (void*)shape;
		sphereCollider.Rigidbody = (void*)actor;

		rb.Body = actor;

		physxScene->GetScene()->addActor(*actor);
	}

	void SphereColliderComponent::Destroy()
	{
		physx::PxShape* shape = (physx::PxShape*)this->Shape;
		physx::PxRigidActor* actor = (physx::PxRigidActor*)Rigidbody;
		actor->detachShape(*shape);

		Shape = nullptr;
		Rigidbody = nullptr;
	}

	void CapsuleColliderComponent::Create(Entity entity, PhysicsScene* physxScene)
	{
	}

	void CapsuleColliderComponent::Destroy()
	{

	}

}