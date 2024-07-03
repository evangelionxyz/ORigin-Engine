// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include <glm/glm.hpp>

namespace origin {

	class TransformComponent;

	class RigidbodyComponent
	{
	public:

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;

		void ApplyForce(glm::vec3 force);
		void ApplyVelocityForce(glm::vec3 force);
		void ApplyAccelerationForce(glm::vec3 force);
		void ApplyImpulseForce(glm::vec3 force);

		void ApplyLinearVelocity(glm::vec3 velocity, bool autoWake = false);

		void SetPosition(glm::vec3 position);
		void SetTransform(glm::vec3 position, glm::quat rotation);

		void UpdateFlags(const TransformComponent& transform);

		void* Body = nullptr;

		bool UseGravity = true;
		bool RotateX = true, RotateY = true, RotateZ = true;
		bool MoveX = true, MoveY = true, MoveZ = true;
		bool Kinematic = false;
		bool RetainAcceleration = false;
		float Mass = 1.0f;
		glm::vec3 CenterMassPosition = glm::vec3(0.0f);

	private:
		RigidbodyComponent(void* body);

		friend class PhysXScene;
		friend class BoxColliderComponent;
		friend class CapsuleColliderComponent;
		friend class SphereColliderComponent;
	};

}

