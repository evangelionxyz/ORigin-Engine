#include "pch.h"
#include "RigidbodyComponent.h"

#include "PxPhysicsAPI.h"

#include "PhysXAPI.h"

namespace origin {

	void RigidbodyComponent::ApplyForce(glm::vec3 force)
	{
		if (!Body)
			return;

		physx::PxRigidActor* actor = (physx::PxRigidActor*)Body;
		physx::PxRigidDynamic* dActor = actor->is<physx::PxRigidDynamic>();
		dActor->addForce(physx::PxVec3(force.x, force.y, force.z), physx::PxForceMode::eFORCE);
	}

	void RigidbodyComponent::ApplyVelocityForce(glm::vec3 force)
	{
		if (!Body)
			return;

		physx::PxRigidActor* actor = (physx::PxRigidActor*)Body;
		physx::PxRigidDynamic* dActor = actor->is<physx::PxRigidDynamic>();
		dActor->addForce(physx::PxVec3(force.x, force.y, force.z), physx::PxForceMode::eVELOCITY_CHANGE);
	}

	void RigidbodyComponent::ApplyAccelerationForce(glm::vec3 force)
	{
		if (!Body)
			return;

		physx::PxRigidActor* actor = (physx::PxRigidActor*)Body;
		physx::PxRigidDynamic* dActor = actor->is<physx::PxRigidDynamic>();
		dActor->addForce(physx::PxVec3(force.x, force.y, force.z), physx::PxForceMode::eACCELERATION);
	}

	void RigidbodyComponent::ApplyImpulseForce(glm::vec3 force)
	{
		if (!Body)
			return;

		physx::PxRigidActor* actor = (physx::PxRigidActor*)Body;
		physx::PxRigidDynamic* dActor = actor->is<physx::PxRigidDynamic>();
		dActor->addForce(physx::PxVec3(force.x, force.y, force.z), physx::PxForceMode::eIMPULSE);
	}

	void RigidbodyComponent::ApplyLinearVelocity(glm::vec3 velocity, bool autoWake)
	{
		if (!Body)
			return;

		physx::PxRigidActor* actor = (physx::PxRigidActor*)Body;
		physx::PxRigidDynamic* dActor = actor->is<physx::PxRigidDynamic>();
		dActor->setLinearVelocity(physx::PxVec3(velocity.x, velocity.y, velocity.z), autoWake);
	}

	void RigidbodyComponent::SetPosition(glm::vec3 position)
	{
		if (!Body)
			return;

		physx::PxRigidActor* actor = (physx::PxRigidActor*)Body;
		if (Kinematic)
			actor->is<physx::PxRigidDynamic>()->setKinematicTarget(physx::PxTransform(Utils::ToPhysXVec3(position)));
		else
			actor->is <physx::PxRigidDynamic>()->setGlobalPose(physx::PxTransform(Utils::ToPhysXVec3(position)));
	}

	void RigidbodyComponent::SetTransform(glm::vec3 position, glm::quat rotation)
	{
		if (!Body)
			return;

		physx::PxRigidActor* actor = (physx::PxRigidActor*)Body;
		if (Kinematic)
			actor->is<physx::PxRigidDynamic>()->setKinematicTarget(physx::PxTransform(Utils::ToPhysXVec3(position), Utils::ToPhysXQuat(rotation)));
		else
			actor->is<physx::PxRigidDynamic>()->setGlobalPose(physx::PxTransform(Utils::ToPhysXVec3(position), Utils::ToPhysXQuat(rotation)));
	}

	void RigidbodyComponent::UpdateFlags(const TransformComponent& transform)
	{
		if (!Body)
			return;

		physx::PxRigidActor* actor = (physx::PxRigidActor*)Body;
		actor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !UseGravity);

		physx::PxRigidDynamic* dActor = actor->is<physx::PxRigidDynamic>();

		dActor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, Kinematic);

		if (!Kinematic)
			dActor->setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eRETAIN_ACCELERATIONS, RetainAcceleration);

		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, !RotateX);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, !RotateY);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, !RotateZ);

		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, !MoveX);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, !MoveY);
		dActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, !MoveZ);

		glm::quat quatRotation = glm::quat(transform.Rotation);

		physx::PxRigidBodyExt::updateMassAndInertia(*dActor, Mass, &Utils::ToPhysXVec3(CenterMassPosition));
		actor->is<physx::PxRigidDynamic>()->setGlobalPose(physx::PxTransform(
			Utils::ToPhysXVec3(transform.Translation), 
			Utils::ToPhysXQuat(quatRotation))
		);
	}

	RigidbodyComponent::RigidbodyComponent(void* body)
	{
		this->Body = body;
	}
}