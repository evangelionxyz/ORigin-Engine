#pragma once

#include "PhysXScene.h"

namespace origin {

	class Collider
	{
	public:
		float StaticFriction = 0.6f;
		float DynamicFriction = 0.6f;
		float Restitution = 0.0f;

	protected:
		void* Shape = nullptr;
		void* Rigidbody = nullptr;

		virtual void Create(Entity entity, PhysicsScene* physxScene) = 0;
		virtual void Destroy() = 0;
	};

	class BoxColliderComponent : public Collider
	{
	public:
		BoxColliderComponent() = default;
		BoxColliderComponent(const BoxColliderComponent&) = default;

		glm::vec3 Offset = glm::vec3(0.0f);
		glm::vec3 Size = glm::vec3(0.5f);

		void Create(Entity entity, PhysicsScene* physxScene) override;
		void Destroy() override;

	private:
		friend class Scene;
		friend class PhysXScene;
		friend class JoltScene;
		friend class RigidbodyComponent;
	};

	class SphereColliderComponent : public Collider
	{
	public:
		SphereColliderComponent() = default;
		SphereColliderComponent(const SphereColliderComponent&) = default;

		glm::vec3 Offset = glm::vec3(0.0f);
		float Radius = 0.5f;

		void Create(Entity entity, PhysicsScene* physxScene) override;
		void Destroy() override;
		
	private:
		friend class Scene;
		friend class PhysXScene;
		friend class JoltScene;
		friend class RigidbodyComponent;
	};

	class CapsuleColliderComponent : public Collider
	{
	public:
		CapsuleColliderComponent() = default;
		CapsuleColliderComponent(const CapsuleColliderComponent&) = default;

		glm::vec3 Offset = glm::vec3(0.0f);
		float Radius = 1.0f;
		float Height = 2.0f;

		// if True = height * scale.y, else = height * scale.x
		bool Horizontal = false;

		void Create(Entity entity, PhysicsScene* physxScene) override;
		void Destroy() override;

	private:
		friend class Scene;
		friend class PhysXScene;
		friend class JoltScene;
		friend class RigidbodyComponent;
	};

	class MeshColliderComponent : public Collider
	{
	public:
		MeshColliderComponent() = default;
		MeshColliderComponent(const MeshColliderComponent&) = default;

		// TODO: Add mesh
	private:
		friend class Scene;
		friend class PhysXScene;
		friend class JoltScene;
		friend class RigidbodyComponent;
	};
}


