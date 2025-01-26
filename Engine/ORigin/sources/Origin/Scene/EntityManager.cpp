// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "EntityManager.h"
#include "Origin/Renderer/Lighting/Lighting.hpp"

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
		Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Camera, scene);
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

	Entity EntityManager::CreateDirectionalLighting(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Lighting, scene);
		entity.AddComponent<DirectionalLightComponent>().Light = Lighting::Create<DirectionalLight>();
		return entity;
	}

	Entity EntityManager::CreateEntityWithUUID(UUID uuid, const std::string &name, const EntityType type, Scene *scene)
	{
		Entity entity = { scene->m_Registry.create(), scene};
		entity.AddComponent<IDComponent>(uuid).Type = type;
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>().Tag = name;
		scene->m_EntityStorage.emplace_back(uuid, entity);
		return entity;
	}

	Entity EntityManager::CreateMesh(const std::string &name, Scene *scene)
	{
		Entity entity = CreateEntityWithUUID(UUID(), name, EntityType::Mesh, scene);
		entity.AddComponent<MeshComponent>();
		entity.AddComponent<RigidbodyComponent>();
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
		auto &destination_id_comp = destination.GetComponent<IDComponent>();
		auto &source_id_comp = source.GetComponent<IDComponent>();

		if (!IsParent(destination_id_comp.ID, source_id_comp.ID, scene))
		{
			if (source_id_comp.Parent != 0)
			{
				Entity current_parent = scene->GetEntityWithUUID(source_id_comp.Parent);
				current_parent.GetComponent<IDComponent>().RemoveChild(source_id_comp.ID);
			}

			destination_id_comp.AddChild(source_id_comp.ID);
			source_id_comp.SetParent(destination_id_comp.ID);
		}
	}

	bool EntityManager::ChildExists(Entity destination, Entity source, Scene *scene)
	{
		auto &destination_id_comp = destination.GetComponent<IDComponent>();
		auto &source_id_comp = source.GetComponent<IDComponent>();

		if (source_id_comp.Parent == destination_id_comp.ID)
		{
			return true;
		}
		auto next_parent = scene->GetEntityWithUUID(source_id_comp.Parent);
		if (ChildExists(next_parent, source, scene))
			return true;
		return false;
	}

	bool EntityManager::IsParent(UUID target, UUID source, Scene *scene)
	{
		Entity destEntity = scene->GetEntityWithUUID(target);
		if (!destEntity.IsValid())
			return false;
		
		const auto &destination_id_comp = destEntity.GetComponent<IDComponent>();
		
		if (target == source)
			return true;
			
		if (destination_id_comp.Parent && IsParent(destination_id_comp.Parent, source, scene))
			return true;
		
		return false;
	}

	Entity EntityManager::FindChild(Entity parent, UUID uuid, Scene *scene)
	{
		if (IsParent(parent.GetUUID(), uuid, scene))
			return scene->GetEntityWithUUID(uuid);
		
		return {entt::null, nullptr};
	}

}