#include "pch.h"
#include "EntityManager.h"

#include "Lighting.h"

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

	Entity EntityManager::CreateLighting(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, scene);
		entity.AddComponent<LightComponent>().Light = Lighting::Create(LightingType::Directional);
		return entity;
	}

	Entity EntityManager::CreateEntityWithUUID(UUID uuid, const std::string &name, Scene *scene)
	{
		Entity entity = { scene->m_Registry.create(), scene};
		entity.AddComponent<IDComponent>(uuid);
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>().Tag = name;
		scene->m_EntityStorage.push_back({ uuid, entity });
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
		std::string name = entity.GetTag();
		Entity newEntity = CreateEntity(name, scene);
		CopyComponentIfExists(AllComponents {}, newEntity, entity);

		auto &entityIDC = entity.GetComponent<IDComponent>();
		auto &newEntityIDC = newEntity.GetComponent<IDComponent>();

		if (entity.HasParent())
		{
			auto &parentIDC = scene->GetEntityWithUUID(entityIDC.Parent).GetComponent<IDComponent>();
			newEntityIDC.Parent = entityIDC.Parent;
		}

		return newEntity;
	}

	void EntityManager::AddChild(Entity destination, Entity source, Scene *scene)
	{
		auto &destIDC = destination.GetComponent<IDComponent>();
		auto &srcIDC = source.GetComponent<IDComponent>();

		if(!IsParent(destIDC.ID, srcIDC.ID, scene))
			srcIDC.Parent = destIDC.ID;
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
		auto &destIDC = destEntity.GetComponent<IDComponent>();

		if (target == source)
			return true;
		if (destIDC.Parent != 0)
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
		return {};
	}

}