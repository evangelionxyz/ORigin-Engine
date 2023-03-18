// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "EditorCamera.h"
#include "Origin\Utils\Time.h"
#include "Origin\Scene\Component\UUID.h"
#include "Origin\Scene\Component\Component.h"

#include "Origin\Renderer\Texture.h"

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

		// 2D Entity
		Entity CreateEntity(const std::string& name = std::string());
		Entity CreateCamera(const std::string& name = std::string());
		Entity CreateCircle(const std::string& name = std::string());
		Entity CreateSpriteEntity(const std::string& name = std::string());

		// 3D Entity
		Entity CreateCube(const std::string& name = std::string());

		// Global Entity
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
		Entity CreateLighting(const std::string& name = std::string());
		Entity GetPrimaryCameraEntity();

		void DestroyEntity(Entity entity);

		void OnUpdateGame(Timestep time);

		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnUpdateRuntime(Timestep time);

		void OnSimulationStart();
		void OnSimulationStop();
		void OnUpdateSimulation(Timestep time, EditorCamera& camera);

		void OnUpdateEditor(Timestep time, EditorCamera& camera);

		void OnViewportResize(uint32_t width, uint32_t height);
		void DuplicateEntity(Entity entity);

		void DrawGrid(int size = 5, glm::vec4 color = glm::vec4(1.0f));
		void SetGrid(int size, glm::vec4 color);

		Entity GetEntityWithUUID(UUID uuid);

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:

		// 2D Scene
		void OnPhysics2DStart();
		void OnPhysics2DStop();

		void RenderScene(EditorCamera& camera);
		void RenderScene(Camera* camera, glm::mat4& transform);

		template<typename T> void OnComponentAdded(Entity entity, T& component);
		int m_GridSize = 5;
		glm::vec4 m_GridColor = glm::vec4(1.0f);
		void DrawIcon(EditorCamera& camera, int entity, std::shared_ptr<Texture2D>& texture, TransformComponent& tc, bool rotate = true);

		bool isRunning() const { return m_Running; }

	private:
		std::unordered_map<UUID, entt::entity> m_EntityMap;
		std::shared_ptr<Texture2D> m_CameraIcon, m_LightingIcon;

		entt::registry m_Registry;
		uint32_t m_ViewportWidth, m_ViewportHeight = 0;
		b2World* m_Box2DWorld;
		bool m_Running = false;

		friend class Entity;
		friend class Editor;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}