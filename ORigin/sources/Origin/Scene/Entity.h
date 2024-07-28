// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "entt/entt.hpp"
#include "Scene.h"

#include "Components/PhysicsComponents.h"
#include "Components/Components.h"

namespace origin {

	class Scene;
	class Entity
	{
	public:
		Entity();
		Entity(entt::entity handle, Scene* scene);
		Entity(const Entity& other) = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			OGN_CORE_ASSERT(!HasComponent<T>(), "[Entity - AddComponent] Entity already has component!");
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
			return m_Scene->m_Registry.all_of<T>(m_EntityHandle);
		}

		template<typename T>
		void RemoveComponent()
		{
			OGN_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
			m_Scene->m_Registry.remove<T>(m_EntityHandle);
		}

		UUID GetUUID() { return GetComponent<IDComponent>().ID; }
		std::string& GetTag() { return GetComponent<TagComponent>().Tag; }
		bool HasParent() { return GetComponent<IDComponent>().Parent != 0; }

		bool IsValid()
		{
			if (m_Scene)
			{
				return m_Scene->m_Registry.valid(m_EntityHandle);
			}
			return m_Scene != nullptr && m_EntityHandle != entt::null;
		}

		operator entt::entity() const { return m_EntityHandle; }
		operator int() const { return static_cast<int>(m_EntityHandle); }
		operator uint32_t() const { return static_cast<uint32_t>(m_EntityHandle); }
		operator uintptr_t() const { return static_cast<uintptr_t>(m_EntityHandle); }

		bool operator==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
		bool operator!=(const Entity& other) const { return !(*this == other); }
		
		Scene* GetScene() const { return m_Scene; }

	private:
		entt::entity m_EntityHandle{entt::null};
		Scene* m_Scene = nullptr;
	};
}
