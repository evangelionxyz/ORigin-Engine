// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "SceneHierarchyPanel.h"

#include "Origin\Utils\log.h"
#include "Origin\IO\Input.h"
#include "Origin\IO\KeyCodes.h"
#include "Origin\Renderer\Texture.h"
#include "Origin\Scene\Component\Component.h"

#include "Origin\Scripting\ScriptEngine.h"


namespace Origin {

	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::DestroyEntity(Entity entity)
	{
		m_Context->DestroyEntity(entity);
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		// Hierarchy
		ImGui::Begin("Hierarchy");

		if (ImGui::IsWindowFocused()) m_HierarchyMenuActive = true;

		if (m_Context)
		{
			m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID, m_Context.get() };
				DrawEntityNode(entity);
			});

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_SelectionContext = {};

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

			if (m_HierarchyMenuActive)
			{
				if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
				{
					if (ImGui::BeginMenu("Create"))
					{
						if (ImGui::MenuItem("Empty"))
							m_Context->CreateEntity("Empty");

						if (ImGui::MenuItem("Camera"))
							m_Context->CreateCamera("Camera");

						if (ImGui::BeginMenu("2D"))
						{
							if (ImGui::MenuItem("Sprite"))
								m_Context->CreateSpriteEntity();
							if (ImGui::MenuItem("Circle"))
								m_Context->CreateCircle("Circle");

							ImGui::EndMenu();
						}

						ImGui::EndMenu();
					}
					ImGui::EndPopup();
				}
			}

			ImGui::PopStyleVar();
		}
		ImGui::End(); // !Hierarchy

		// Properties
		ImGui::Begin("Properties");
		if (m_SelectionContext)
			DrawComponents(m_SelectionContext);
		ImGui::End(); // !Properties
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tagComponent = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tagComponent.c_str());

		if (ImGui::IsItemClicked()) m_SelectionContext = entity;

		// destroy entity
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				m_Context->DestroyEntity(entity);
				m_SelectionContext = {};
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tagComponent.c_str());
			if(opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
				if (tag.empty()) tag = "'No Name'";
			}
		}

		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			DisplayAddComponentEntry<ScriptComponent>("Script");
			DisplayAddComponentEntry<CameraComponent>("Camera");
			DisplayAddComponentEntry<SpriteRendererComponent>("Sprite Renderer");
			DisplayAddComponentEntry<SpriteRenderer2DComponent>("Sprite Renderer 2D");
			DisplayAddComponentEntry<LightingComponent>("Lighting");
			DisplayAddComponentEntry<CircleRendererComponent>("Circle Renderer");
			DisplayAddComponentEntry<NativeScriptComponent>("C++ Native Script");
			DisplayAddComponentEntry<Rigidbody2DComponent>("Rigidbody 2D");
			DisplayAddComponentEntry<BoxCollider2DComponent>("Box Collider 2D");
			DisplayAddComponentEntry<CircleCollider2DComponent>("Circle Collider 2D");

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Translation);

		glm::vec3 rotation = glm::degrees(component.Rotation);
		DrawVec3Control("Rotation", rotation, 1.0f);
		component.Rotation = glm::radians(rotation);

		DrawVec3Control("Scale", component.Scale, 0.01f, 1.0f);
			});

		DrawComponent<ScriptComponent>("Script", entity, [](auto& component)
			{
				bool scriptClassExist = ScriptEngine::EntityClassExists(component.ClassName);

				if (!scriptClassExist)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

				static char buffer[64];
				strcpy(buffer, component.ClassName.c_str());
				if (ImGui::InputText("Script Class", buffer, sizeof(buffer)))
					component.ClassName = std::string(buffer);

				if (!scriptClassExist)
					ImGui::PopStyleColor();

			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				if (!component.Texture)
					ImGui::Button("Drop Texture", ImVec2(80.0f, 30.0f));
				else ImGui::ImageButton((ImTextureID)component.Texture->GetRendererID(), ImVec2(80.0f, 80.0f), ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
						if (texturePath.extension() == ".png" || texturePath.extension() == ".jpg")
							component.Texture = Texture2D::Create(texturePath.string());
					}
				}

				if (component.Texture)
				{
					ImGui::SameLine();
					if (ImGui::Button("Delete", ImVec2(80.0f, 30.0f)))
					{
						component.Texture->Delete();
						component.Texture = {};
						return;
					}

					ImGui::Text("Path: %s", component.Texture->GetFilepath().c_str());
				}
			});

		DrawComponent<SpriteRenderer2DComponent>("Sprite Renderer 2D", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				if (!component.Texture)
					ImGui::Button("Drop Texture", ImVec2(80.0f, 30.0f));
				else ImGui::ImageButton((ImTextureID)component.Texture->GetRendererID(), ImVec2(80.0f, 80.0f), ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
						if (texturePath.extension() == ".png" || texturePath.extension() == ".jpg")
							component.Texture = Texture2D::Create(texturePath.string());
					}
				}

				if (component.Texture)
				{
					ImGui::SameLine();
					if (ImGui::Button("Delete", ImVec2(80.0f, 30.0f)))
					{
						component.Texture->Delete();
						component.Texture = {};
						return;
					}

					ImGui::Text("Path: %s", component.Texture->GetFilepath().c_str());
					ImGui::DragFloat("Tilling Factor", &component.TillingFactor, 0.1f, 0.0f, 10.0f);
				}
			});
		DrawComponent<LightingComponent>("Lighting", entity, [](auto& component)
			{
				ImGui::ColorPicker4("Color", glm::value_ptr(component.Color),
				ImGuiColorEditFlags_DisplayRGB
				| ImGuiColorEditFlags_InputHSV
				);
			});

		DrawComponent<CircleRendererComponent>("Circle", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
		ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
		ImGui::DragFloat("Fade", &component.Fade, 0.025f, 0.0f, 1.0f);

			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

		const char* projectionTypeString[] = { "Perspective", "Orthographic" };
		const char* currentProjectionTypeString = projectionTypeString[(int)component.Camera.GetProjectionType()];

		ImGui::Checkbox("Primary", &component.Primary);

		if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
		{

			for (int i = 0; i < 2; i++)
			{
				bool isSelected = currentProjectionTypeString == projectionTypeString[i];
				if (ImGui::Selectable(projectionTypeString[i], isSelected))
				{
					currentProjectionTypeString = projectionTypeString[i];
					component.Camera.SetProjectionType((SceneCamera::ProjectionType)i);
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (component.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
		{
			float perspectiveFov = glm::degrees(camera.GetPerspectiveFov());
			camera.SetPerspectiveFov(glm::radians(perspectiveFov));

			if (ImGui::DragFloat("FOV", &perspectiveFov, 0.1f, 0.01f, 10000.0f))
				camera.SetPerspectiveFov(glm::radians(perspectiveFov));

			float perspectiveNearClip = camera.GetPerspectiveNearClip();
			if (ImGui::DragFloat("Near Clip", &perspectiveNearClip, 0.1f))
				camera.SetPerspectiveNearClip(perspectiveNearClip);

			float perspectiveFarClip = camera.GetPerspectiveFarClip();
			if (ImGui::DragFloat("Far Clip", &perspectiveFarClip, 0.1f))
				camera.SetPerspectiveFarClip(perspectiveFarClip);
		}

		if (component.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
		{
			float orthoSize = camera.GetOrthographicSize();
			camera.SetOrthographicSize(orthoSize);

			if (ImGui::DragFloat("Ortho Size", &orthoSize, 0.1f, 1.0f, 100.0f))
				camera.SetOrthographicSize(orthoSize);

			float orthoNearClip = camera.GetOrthographicNearClip();
			if (ImGui::DragFloat("Near Clip", &orthoNearClip, 0.1f, -1.0f, 10.0f))
				camera.SetOrthographicNearClip(orthoNearClip);

			float orthoFarClip = camera.GetOrthographicFarClip();
			if (ImGui::DragFloat("Far Clip", &orthoFarClip, 0.1f, 10.0f, 100.0f))
				camera.SetOrthographicFarClip(orthoFarClip);

			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
		}});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
		{
			const char* bodyTypeString[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeString[(int)component.Type];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeString[i];
					if (ImGui::Selectable(bodyTypeString[i], isSelected))
					{
						currentBodyTypeString = bodyTypeString[i];
						component.Type = (Rigidbody2DComponent::BodyType)i;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);

			});

		DrawComponent<BoxCollider2DComponent>("Box 2D Collider", entity, [](auto& component)
			{
				DrawVec2Control("Offset", component.Offset, 0.01f, 0.0f);
				DrawVec2Control("Size", component.Size, 0.01f, 0.0f);

				float width = 118.0f;
				DrawVecControl("Density", &component.Density, 0.01f, 1.0f, width);
				DrawVecControl("Friction", &component.Friction, 0.01f, 0.5f, width);
				DrawVecControl("Restitution", &component.Restitution, 0.01f, 0.0f, width);
				DrawVecControl("Restitution Thrs", &component.RestitutionThreshold, 0.01f, 1.0f, width);
			});

		DrawComponent<CircleCollider2DComponent>("Circle 2D Collider", entity, [](auto& component)
			{
				DrawVec2Control("Offset", component.Offset, 0.01f, 0.0f);
				DrawVecControl("Radius", &component.Radius, 0.01f, 0.5f);

				float width = 118.0f;
				DrawVecControl("Density", &component.Density, 0.01f, 0.5f, width);
				DrawVecControl("Friction", &component.Friction, 0.01f, 0.0f, width);
				DrawVecControl("Restitution", &component.Restitution, 0.01f, 0.0f, width);
				DrawVecControl("Restitution Thrs", &component.Restitution, 0.01f, 0.5f, width);
			});
	}

	template<typename T>
	void SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName) {
		if (!m_SelectionContext.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectionContext.AddComponent<T>();
				ImGui::CloseCurrentPopup();
			}
		}
	}

}