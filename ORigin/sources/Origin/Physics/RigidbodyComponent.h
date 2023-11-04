#pragma once

namespace origin {

	class RigidbodyComponent
	{
	public:
		enum class ForceMode {
			Force,
			Impulse,
			Velocity,
			Acceleration
		};

		RigidbodyComponent() = default;
		RigidbodyComponent(const RigidbodyComponent&) = default;

		void AddForce(glm::vec3 force, ForceMode);
		void SetPosition(glm::vec3 position);
		void SetTransform(glm::vec3 position, glm::quat rotation);

		void UpdateFlags();

		void* GetRuntimeBody() { return Body; }

		bool UseGravity = true;
		bool RotateX = true, RotateY = true, RotateZ = true;
		bool MoveX = true, MoveY = true, MoveZ = true;
		bool Kinematic = false;
		bool RetainAcceleration = false;
		float Mass = 1.0f;
		glm::vec3 CenterMassPosition = glm::vec3(0.0f);

	private:
		void* Body = nullptr;
		RigidbodyComponent(void* body);

		friend class PhysXScene;
		friend class BoxColliderComponent;
		friend class CapsuleColliderComponent;
		friend class SphereColliderComponent;
	};

}

