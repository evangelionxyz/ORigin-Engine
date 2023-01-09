// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Component\UUID.h"
#include "Component\Component.h"

#include "Scene.h"
#include "entt\entt.hpp"

namespace Origin {

	/*template<typename T> const char* ComponentToString()
	{
		switch (T)
		{
			case SpriteRendererComponent: return "Sprite Renderer"; break;
			case CircleRendererComponent: return "Circle Renderer"; break;
			case CameraComponent: return "Camera"; break;
			case NativeScriptComponent: return "Native C++ Script"; break;
			case TransformComponent: return "Transform"; break;
			case Rigidbody2DComponent: return "Rigidbody 2D"; break;
			case BoxCollider2DComponent: return "Box Collider 2D"; break;
			case TagComponent: return "Tag"; break;
			case IDComponent: return "ID"; break;

		}
	}*/

	class Entity
	{
	public:
		Entity() = default;
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		~Entity();

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			OGN_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
			T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T, typename... Args>
		T& AddOrReplaceComponent(Args&&... args)
		{
			T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_EntityHandle, std::forward<Args>(args)...);
			m_Scene->OnComponentAdded<T>(*this, component);
			return component;
		}

		template<typename T>
		T& GetComponent()
		{
			OGN_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			return m_Scene->m_Registry.get<T>(m_EntityHandle);
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			OGN_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator entt::entity() const { return m_EntityHandle; }
		operator uint32_t() const { return (uint32_t)m_EntityHandle; }

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		std::string& GetName() { return GetComponent<TagComponent>().Tag; }
		int GetCount() { return Count; }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

	private:
		entt::entity m_EntityHandle{ entt::null };
		Scene* m_Scene = nullptr;
		int Count = 0;
	};
}
