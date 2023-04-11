// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Scene\Scene.h"
#include "Origin\Scene\Entity.h"

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace Origin
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const std::shared_ptr<Scene>& context);

		Entity SetSelectedEntity(Entity entity);

		Entity GetSelectedEntity() const { return m_SelectedEntity; }
		void SetHierarchyMenuActive(bool enable) { m_HierarchyMenuActive = enable; }
		bool GetHierarchyMenuActive() { return m_HierarchyMenuActive; }
		void SetContext(const std::shared_ptr<Scene>& context, bool reset = false);
		void DestroyEntity(Entity entity);
		std::shared_ptr<Scene> GetContext() { return m_Context; }

		void OnImGuiRender();
	private:
		template<typename T>
		void DisplayAddComponentEntry(const std::string& entryName);

		bool m_HierarchyMenuActive = false;
		void DrawEntityNode(Entity entity);
		void DrawComponents(Entity entity);
	private:
		std::shared_ptr<Scene> m_Context;
		Entity m_SelectedEntity;

		std::shared_ptr<Texture2D> m_NoTextureButton;
	};

	// static function
	static void DrawVec3Control(const std::string& label, glm::vec3& values, float speed = 0.025f, float resetValue = 0.0f, float coloumnWidth = 80.0f)
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

		// ================ X ================
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PopStyleColor(3);

		// ================ Y ================
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.8f, 0.1f, 1.0f));
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PopStyleColor(3);

		// ================ Z ================
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.8f, 1.0f));
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, speed);
		ImGui::PopItemWidth();
		ImGui::PopStyleColor(3);

		ImGui::PopStyleVar(1);
		ImGui::Columns(1);

		ImGui::PopID();
	}

	// static function
	static void DrawVec2Control(const std::string& label, glm::vec2& values, float speed = 0.025f, float resetValue = 0.0f, float coloumnWidth = 80.0f)
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

		// ================ X ================
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PopStyleColor(3);

		// ================ Y ================
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.6f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.8f, 0.1f, 1.0f));
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, speed);
		ImGui::PopItemWidth();
		ImGui::PopStyleColor(3);

		ImGui::PopStyleVar(1);
		ImGui::Columns(1);

		ImGui::PopID();
	}

	// static function
	static void DrawVecControl(const std::string& label, float* values, float speed = 0.025f, float resetValue = 0.0f, float coloumnWidth = 80.0f)
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, coloumnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

		float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = ImVec2(lineHeight + 3.0f, lineHeight);

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		if (ImGui::Button("V", buttonSize))
			*values = resetValue;
		ImGui::SameLine();
		ImGui::DragFloat("##V", values, speed);
		ImGui::PopItemWidth();
		ImGui::PopStyleColor(3);

		ImGui::PopStyleVar(1);
		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvailable.x - lineHeight);
			if (ImGui::Button("+", ImVec2(lineHeight, lineHeight)))
				ImGui::OpenPopup("Component Settings");

			bool removeComponent = false;
			if (ImGui::BeginPopup("Component Settings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

}
