// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Scene\Scene.h"
#include "Origin\Scene\Entity.h"

#include <filesystem>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>


namespace origin {

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string &name, Entity entity, UIFunction uiFunction);

	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const std::shared_ptr<Scene>& context);

		~SceneHierarchyPanel();

		static SceneHierarchyPanel& Get() { return *s_Instance; }
		std::shared_ptr<Scene> GetContext() { return m_Context; }

		Entity SetSelectedEntity(Entity entity);
		Entity GetSelectedEntity() const;

		void OnImGuiRender();

		void EntityHierarchyPanel();
		void EntityPropertiesPanel();

		void SetContext(const std::shared_ptr<Scene>& context, bool reset = false);
		void DestroyEntity(Entity entity);

		bool IsSceneHierarchyFocused = false;
		bool IsScenePropertiesFocused = false;

	private:
		static SceneHierarchyPanel* s_Instance;

		template<typename T>
		bool DisplayAddComponentEntry(const std::string& entryName);
		void DrawEntityNode(Entity entity, int index = 0);
		void DrawComponents(Entity entity);
		void AddNodeChild(Entity parent, Entity child);
		std::shared_ptr<Scene> m_Context;
		Entity m_SelectedEntity;
		std::shared_ptr<Texture2D> m_NoTextureButton;
	};
}
