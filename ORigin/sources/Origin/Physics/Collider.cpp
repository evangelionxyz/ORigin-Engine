#include "pch.h"
#include "Collider.h"

#include "PhysXAPI.h"

#include "PxRigidActor.h"

#include "Origin\Scene\Entity.h"

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
				actor = physxScene->CreateActor(rb, tc.Translation, tc.WorldRotation);
		}

		else
		{
			RigidbodyComponent& rb = entity.AddComponent<RigidbodyComponent>();
			rb.UseGravity = true;
			rb.Kinematic = true;
			actor = physxScene->CreateActor(rb, tc.Translation, tc.WorldRotation);
		}

		RigidbodyComponent& rb = entity.GetComponent<RigidbodyComponent>();
		physx::PxTransform relativePos(Utils::ToPhysXVec3(boxCollider.Offset));
		physx::PxMaterial* pxMaterial = PhysXAPI::GetPhysics()->createMaterial(boxCollider.StaticFriction, boxCollider.DynamicFriction, boxCollider.Restitution);

		physx::PxShape* shape = physx::PxRigidActorExt::createExclusiveShape(*actor, physx::PxBoxGeometry(Utils::ToPhysXVec3(boxCollider.Size)), *pxMaterial);

		shape->setLocalPose(relativePos);
		boxCollider.Shape = (void*)shape;
		boxCollider.Rigidbody = (void*)actor;

		rb.Body = actor;

		physxScene->GetScene()->addActor(*actor);
	}

	void BoxColliderComponent::Destroy()
	{
		if (Rigidbody && Shape)
		{
			physx::PxShape* shape = (physx::PxShape*)this->Shape;
			physx::PxRigidActor* actor = (physx::PxRigidActor*)Rigidbody;
			actor->detachShape(*shape);
		}

		Shape = nullptr;
		Rigidbody = nullptr;
	}

	void SphereColliderComponent::Create(Entity entity, PhysicsScene* physxScene)
	{

	}

	void SphereColliderComponent::Destroy()
	{

	}

	void CapsuleColliderComponent::Create(Entity entity, PhysicsScene* physxScene)
	{
	}

	void CapsuleColliderComponent::Destroy()
	{

	}

}