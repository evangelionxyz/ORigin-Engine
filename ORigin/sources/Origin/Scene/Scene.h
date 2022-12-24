// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Utils\Time.h"
#include "EditorCamera.h"
#include "Origin/Scene/Component/UUID.h"

#include "entt\entt.hpp"

class b2World;

namespace Origin
{
	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		static std::shared_ptr<Scene> Copy(std::shared_ptr<Scene> other);

		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		Entity CreateCamera(const std::string& name = std::string());
		Entity CreateCircle(const std::string& name = std::string());
		Entity CreateSpriteEntity(const std::string& name = std::string());
		Entity GetPrimaryCameraEntity();

		void DestroyEntity(Entity entity);

		void OnUpdateRuntime(Timestep time);
		void OnUpdateEditor(Timestep time, EditorCamera& camera);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnViewportResize(uint32_t width, uint32_t height);
		void DuplicateEntity(Entity entity);

		void DrawGrid(int size = 5, glm::vec4 color = glm::vec4(1.0f));
		void SetGrid(int size, glm::vec4 color);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		template<typename T> void OnComponentAdded(Entity entity, T& component);
		int m_GridSize = 5;
		glm::vec4 m_GridColor = glm::vec4(1.0f);

	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth, m_ViewportHeight = 0;
		b2World* m_Box2DWorld;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
		friend class Editor;
	};

	

}