// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Scene/Scene.h"
#include "Origin/Scene/Entity.h"

namespace origin {

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const std::shared_ptr<Scene> &scene);

		~SceneHierarchyPanel();

		std::shared_ptr<Scene> GetContext() { return m_Scene; }

		Entity SetSelectedEntity(Entity entity);
		Entity GetSelectedEntity();

		void OnImGuiRender();

		void EntityHierarchyPanel();
		void EntityPropertiesPanel();

		void SetActiveScene(const std::shared_ptr<Scene> &scene, bool reset = false);
		void DestroyEntity(Entity entity);

		bool IsSceneHierarchyFocused = false;
		bool IsScenePropertiesFocused = false;

	private:
		template<typename T>
		bool DisplayAddComponentEntry(const std::string& entryName);
		template<typename T, typename UIFunction>
		void DrawComponent(const std::string &name, Entity entity, UIFunction uiFunction);

		void DrawEntityNode(Entity entity, int index = 0);
		void DrawComponents(Entity entity);

		Entity EntityContextMenu();

		std::shared_ptr<Scene> m_Scene;
		std::shared_ptr<Texture2D> m_NoTextureButton;

		Entity m_SelectedEntity = Entity();
	};
}
