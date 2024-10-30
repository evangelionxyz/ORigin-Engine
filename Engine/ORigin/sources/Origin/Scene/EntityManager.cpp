// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "EntityManager.h"

#include "Lighting.h"

namespace origin
{
	Entity EntityManager::CreateEntity(const std::string &name, Scene *scene, EntityType type)
	{
		return CreateEntityWithUUID(UUID(), name, type, scene);
	}

	Entity EntityManager::CreateAudio(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Audio, scene);
		entity.AddComponent<AudioComponent>();
		return entity;
	}

	Entity EntityManager::CreateSprite(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Entity, scene);
		entity.AddComponent<SpriteRenderer2DComponent>();
		return entity;
	}

	Entity EntityManager::CreateCircle(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Entity, scene);
		entity.AddComponent<CircleRendererComponent>();
		return entity;
	}

	Entity EntityManager::CreateCamera(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Camera, scene);
		auto cc = entity.AddComponent<CameraComponent>();
		cc.Camera.InitOrthographic(10, 0.1f, 2.0f);
		cc.Camera.InitPerspective(40.0f, 1.7776f, 0.1f, 500.0f);

		entity.AddComponent<AudioListenerComponent>();
		entity.GetComponent<TransformComponent>().Translation.z = 8.0f;
		return entity;
	}

	Entity EntityManager::CreateLighting(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Lighting, scene);
		entity.AddComponent<LightComponent>().Light = Lighting::Create(LightingType::Directional);
		return entity;
	}

	Entity EntityManager::CreateEntityWithUUID(UUID uuid, const std::string &name, EntityType type, Scene *scene)
	{
		Entity entity = { scene->m_Registry.create(), scene};
		entity.AddComponent<IDComponent>(uuid).Type = type;
		TransformComponent &tc = entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>().Tag = name;
		scene->m_EntityStorage.push_back({ uuid, entity });
		return entity;
	}

	Entity EntityManager::CreateMesh(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Mesh, scene);
		entity.AddComponent<StaticMeshComponent>();
		entity.AddComponent<RigidbodyComponent>();
		return entity;
	}

    Entity EntityManager::CreateCube(const std::string &name, Scene *scene)
    {
        Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Mesh, scene);
		entity.AddComponent<StaticMeshComponent>().mType = StaticMeshComponent::Type::Cube;
		entity.AddComponent<RigidbodyComponent>();
		entity.AddComponent<BoxColliderComponent>();
        return entity;
    }

    Entity EntityManager::CreateSphere(const std::string &name, Scene *scene)
    {
        Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Mesh, scene);
        entity.AddComponent<StaticMeshComponent>().mType = StaticMeshComponent::Type::Sphere;
        entity.AddComponent<RigidbodyComponent>();
        entity.AddComponent<SphereColliderComponent>();
        return entity;
    }

    Entity EntityManager::CreateCapsule(const std::string &name, Scene *scene)
    {
        Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Mesh, scene);
        entity.AddComponent<StaticMeshComponent>().mType = StaticMeshComponent::Type::Capsule;
        entity.AddComponent<RigidbodyComponent>();
        entity.AddComponent<CapsuleColliderComponent>();
        return entity;
    }

    Entity EntityManager::CreateUI(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::UI, scene);
		entity.AddComponent<UIComponent>();
		return entity;
	}

	void EntityManager::AddChild(Entity destination, Entity source, Scene *scene)
	{
		auto &destIDC = destination.GetComponent<IDComponent>();
		auto &srcIDC = source.GetComponent<IDComponent>();

		if (!IsParent(destIDC.ID, srcIDC.ID, scene))
		{
			destIDC.AddChild(srcIDC.ID);
			srcIDC.SetParent(destIDC.ID);
		}
	}

	bool EntityManager::ChildExists(Entity destination, Entity source, Scene *scene)
	{
		auto &destIDC = destination.GetComponent<IDComponent>();
		auto &srcIDC = source.GetComponent<IDComponent>();

		if (srcIDC.Parent == destIDC.ID)
			return true;
		else
		{
			auto nextParent = scene->GetEntityWithUUID(srcIDC.Parent);
			if (ChildExists(nextParent, source, scene))
				return true;
		}

		return false;
	}

	bool EntityManager::IsParent(UUID target, UUID source, Scene *scene)
	{
		Entity destEntity = scene->GetEntityWithUUID(target);
		if (!destEntity.IsValid())
			return false;
		auto &destIDC = destEntity.GetComponent<IDComponent>();
		if (target == source)
			return true;
		if (destIDC.Parent)
		{
			if (EntityManager::IsParent(destIDC.Parent, source, scene))
				return true;
		}
		return false;
	}

	Entity EntityManager::FindChild(Entity parent, UUID uuid, Scene *scene)
	{
		if (IsParent(parent.GetUUID(), uuid, scene))
			return scene->GetEntityWithUUID(uuid);
		return {entt::null, nullptr};
	}

}