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
		scene->m_EntityMap[uuid] = entity;
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
			auto &entityIDC = entity.GetComponent<IDComponent>();
			auto &parentIDC = scene->GetEntityWithUUID(entityIDC.Parent).GetComponent<IDComponent>();
			parentIDC.Children.push_back(newEntity.GetUUID());
		}

		return newEntity;
	}

	void EntityManager::AddChild(Entity destination, Entity source, Scene *scene)
	{
		auto &destIDC = destination.GetComponent<IDComponent>();
		auto &srcIDC = source.GetComponent<IDComponent>();

		if (!EntityManager::ChildExists(destination.GetUUID(), source.GetUUID(), scene))
		{
			if (destination.HasParent())
			{
				if (!EntityManager::ParentOrGrandParentExists(destIDC.Parent, source.GetUUID(), scene))
				{
					if (source.HasParent())
					{
						auto &children = scene->GetEntityWithUUID(srcIDC.Parent).GetComponent<IDComponent>().Children;
						children.erase(std::find(children.begin(), children.end(), source.GetUUID()));
					}
					destIDC.Children.push_back(source.GetUUID());
					srcIDC.Parent = destination.GetUUID();
				}
			}
			else
			{
				if (source.HasParent())
				{
					auto &children = scene->GetEntityWithUUID(srcIDC.Parent).GetComponent<IDComponent>().Children;
					children.erase(std::find(children.begin(), children.end(), source.GetUUID()));
				}
				destIDC.Children.push_back(source.GetUUID());
				srcIDC.Parent = destination.GetUUID();
			}
		}
	}

	void EntityManager::DeleteChild(Entity parent, Entity child)
	{
		if (parent.HasChildren())
		{
			auto &parentIDC = parent.GetComponent<IDComponent>();
			parentIDC.Children.erase(std::find(parentIDC.Children.begin(), parentIDC.Children.end(), child.GetUUID()));
		}
	}

	bool EntityManager::ChildExists(UUID destination, UUID source, Scene *scene)
	{
		Entity destEntity = scene->GetEntityWithUUID(destination);
		auto &destIDC = destEntity.GetComponent<IDComponent>();
		for (UUID child : destIDC.Children)
		{
			if (child == source)
				return true;
		}

		return false;
	}

	bool EntityManager::ChildOrGrandChildExists(UUID destination, UUID source, Scene *scene)
	{
		Entity destEntity = scene->GetEntityWithUUID(destination);
		auto &destIDC = destEntity.GetComponent<IDComponent>();

		for (UUID nextChild : destIDC.Children)
		{
			if (nextChild == source)
				return true;

			return ChildOrGrandChildExists(nextChild, source, scene);
		}

		return false;
	}

	bool EntityManager::ParentOrGrandParentExists(UUID destinationParent, UUID source, Scene *scene)
	{
		Entity destEntity = scene->GetEntityWithUUID(destinationParent);
		auto &destIDC = destEntity.GetComponent<IDComponent>();

		if (destinationParent == source)
			return true;

		if(destIDC.Parent != 0)
			if (EntityManager::ParentOrGrandParentExists(destIDC.Parent, source, scene))
				return true;

		return false;
	}

	void EntityManager::DestroyEntityFromScene(Entity entity, Scene *scene)
	{
		auto &children = entity.GetComponent<IDComponent>().Children;

		// destroy children recursively
		for (auto child : children)
		{
			Entity c = scene->GetEntityWithUUID(child);
			EntityManager::DestroyEntityFromScene(c, scene);
		}

		scene->m_EntityMap.erase(entity.GetUUID());
		scene->m_Registry.destroy(entity);
	}
}