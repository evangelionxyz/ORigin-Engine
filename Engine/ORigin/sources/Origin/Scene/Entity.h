// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef ENTITY_H
#define ENTITY_H

#include "entt/entt.hpp"
#include "Origin/Core/ConsoleManager.h"
#include "Components/PhysicsComponents.h"
#include "Components/Components.h"
#include "Scene.h"

namespace origin {

class Entity
{
public:
	Entity()
		:m_handle(entt::null), m_scene(nullptr)
	{
	}

	Entity(entt::entity handle, Scene *scene)
		: m_handle(handle), m_scene(scene)
	{
	}

	Entity(const Entity &other) = default;

	template<typename T, typename... Args>
	T &AddComponent(Args&&... args)
	{
		OGN_CORE_ASSERT(!HasComponent<T>(), "[Entity - AddComponent] Entity already has component!");
		T &component = m_scene->m_Registry.emplace<T>(m_handle, std::forward<Args>(args)...);
		m_scene->OnComponentAdded<T>(*this, component);
		return component;
	}

	template<typename T, typename... Args>
	T &AddOrReplaceComponent(Args&&... args)
	{
		T &component = m_scene->m_Registry.emplace_or_replace<T>(m_handle, std::forward<Args>(args)...);
		m_scene->OnComponentAdded<T>(*this, component);
		return component;
	}

	template<typename T>
	T &GetComponent()
	{
		OGN_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		return m_scene->m_Registry.get<T>(m_handle);
	}

	template<typename T>
	bool HasComponent()
	{
		return m_scene->m_Registry.all_of<T>(m_handle);
	}

	template<typename T>
	void RemoveComponent()
	{
		OGN_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
		m_scene->m_Registry.remove<T>(m_handle);
	}

	UUID GetUUID() { return GetComponent<IDComponent>().ID; }

	UUID GetParentUUID() { return GetComponent<IDComponent>().Parent; }
	bool HasParent() { return GetComponent<IDComponent>().Parent != 0; }

	EntityType GetType() { return GetComponent<IDComponent>().Type; }
	std::string &GetTag() { return GetComponent<TagComponent>().Tag; }
	Scene *GetScene() const { return m_scene; }
	entt::entity GetHandle() { return m_handle; }

	bool IsValid() const
	{
		bool isValid = true;
		if (m_scene)
		{
			if (m_scene->m_Name.empty())
			{
				isValid = false;
			}
			else
			{
				isValid = m_scene->m_Registry.valid(m_handle);
			}
		}
		else
		{
			isValid = m_scene != nullptr && m_handle != entt::null;
		}

		return isValid;
	}

	operator bool() const { return IsValid(); }
	operator entt::entity() const { return m_handle; }
	operator int() const { return static_cast<int>(m_handle); }
	operator uint32_t() const { return static_cast<uint32_t>(m_handle); }
	operator uintptr_t() const { return static_cast<uintptr_t>(m_handle); }
	bool operator==(const Entity &other) const { return m_handle == other.m_handle && m_scene == other.m_scene; }
	bool operator!=(const Entity &other) const { return !(*this == other); }

private:
	entt::entity m_handle{ entt::null };
	Scene *m_scene = nullptr;
};
}

#endif