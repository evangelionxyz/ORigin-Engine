#include "pch.h"
#include "EntityManager.h"

namespace origin
{
	Entity EntityManager::CreateEntity(const std::string &name, Scene *scene)
	{
		return CreateEntityWithUUID(UUID(), name, scene);
	}

	Entity EntityManager::CreateAudio(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, scene);
		entity.AddComponent<AudioComponent>();
		return entity;
	}

	Entity EntityManager::CreateSprite(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, scene);
		entity.AddComponent<SpriteRenderer2DComponent>();
		return entity;
	}

	Entity EntityManager::CreateCircle(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, scene);
		entity.AddComponent<CircleRendererComponent>();
		return entity;
	}

	Entity EntityManager::CreateCamera(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, scene);
		entity.AddComponent<CameraComponent>();
		entity.AddComponent<AudioListenerComponent>();
		entity.GetComponent<TransformComponent>().Translation.z = 8.0f;
		return entity;
	}

	Entity EntityManager::CreateEntityWithUUID(UUID uuid, const std::string &name, Scene *scene)
	{
		Entity entity = { scene->m_Registry.create(), scene};
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TreeNodeComponent>();
		entity.AddComponent<TagComponent>().Tag = name;
		scene->m_EntityMap[entity.GetUUID()] = entity;
		return entity;
	}

	Entity EntityManager::CreateMesh(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, scene);
		entity.AddComponent<StaticMeshComponent>();
		return entity;
	}

	Entity EntityManager::DuplicateEntity(Entity entity, Scene *scene)
	{
		// TODO: Skipped for now
		if (entity.HasChildren())
			return entity;

		std::string name = entity.GetTag();
		Entity newEntity = CreateEntity(name, scene);
		CopyComponentIfExists(AllComponents{}, newEntity, entity);
		if (entity.HasParent())
		{
			Entity parent = scene->GetEntityWithUUID(entity.GetParentUUID());
			auto &parentIdc = parent.GetComponent<TreeNodeComponent>();
			parentIdc.Children[newEntity.GetUUID()] = newEntity;
		}
		return newEntity;
	}

	void EntityManager::DestroyEntity(Entity entity, Scene *scene)
	{
		scene->m_EntityMap.erase(entity.GetUUID());
		scene->m_Registry.destroy(entity);
	}

}