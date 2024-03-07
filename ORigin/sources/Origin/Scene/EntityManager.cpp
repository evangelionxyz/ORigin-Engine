#include "pch.h"
#include "EntityManager.h"

namespace origin
{
	entt::registry EntityManager::m_EditorRegistry;

	Entity EntityManager::CreateEntity(const std::string &name, Scene *scene)
	{
		return CreateEntityWithUUID(UUID(), name, scene);
	}

	Entity EntityManager::CreateSprite(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, scene);
		entity.AddComponent<SpriteRenderer2DComponent>();
		return entity;
	}

	Entity EntityManager::CreateCamera(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, scene);
		entity.AddComponent<CameraComponent>();
		entity.GetComponent<TransformComponent>().Translation.z = 8.0f;
		return entity;
	}

	Entity EntityManager::CreateEntityWithUUID(UUID uuid, const std::string &name, Scene *scene)
	{
		Entity entity = { scene->m_Registry.create(), scene};
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TreeNodeComponent>();

		auto &tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
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

	Entity EntityManager::CreateEditorEntity(const std::string &name)
	{
		Entity entity = { m_EditorRegistry.create(), nullptr };
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>().Tag = name;
		entity.AddComponent<IDComponent>().ID = UUID();
		return entity;
	}

	void EntityManager::DestroyEditorEntity(Entity entity)
	{
		m_EditorRegistry.destroy(entity);
	}

}