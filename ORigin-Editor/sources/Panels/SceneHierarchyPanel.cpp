// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "SceneHierarchyPanel.h"
#include "../EditorLayer.h"
#include "Origin\GUI\UI.h"
#include "Origin\Project\Project.h"
#include "Origin\Asset\AssetManager.h"
#include "Origin\Asset\AssetMetaData.h"
#include "Origin\Scene\EntityManager.h"
#include "Origin\Asset\AssetImporter.h"
#include "Origin\Audio\AudioSource.h"
#include "Origin\Scripting\ScriptEngine.h"
#include "Origin\Renderer\Renderer.h"
#include "Origin\Scene\Lighting.h"
#include "UIEditor.h"

#include "box2d\b2_revolute_joint.h"
#include "box2d\b2_fixture.h"
#include <misc\cpp\imgui_stdlib.h>

#pragma warning(disable : OGN_DISABLED_WARNINGS)

namespace origin {

	static SceneHierarchyPanel* s_Instance = nullptr;

	SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& context)
	{
		OGN_PROFILER_UI();

		SetContext(context);
		s_Instance = this;
	}

	SceneHierarchyPanel::~SceneHierarchyPanel()
	{
		OGN_PROFILER_UI();
	}

	SceneHierarchyPanel *SceneHierarchyPanel::Get()
	{
		return s_Instance;
	}

	Entity SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		return m_SelectedEntity = entity;
	}

	Entity SceneHierarchyPanel::GetSelectedEntity() const
	{
		if(m_SelectedEntity.GetScene())
			return m_SelectedEntity;

		return {};
	}

	void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& context, bool reset)
	{
		if (reset)
			m_SelectedEntity = {};

		m_Context = context;

		if (!m_SelectedEntity)
			return;

		UUID entityID = m_SelectedEntity.GetUUID();
		Entity newEntity = m_Context->GetEntityWithUUID(entityID);

		if (entityID == newEntity.GetUUID())
			m_SelectedEntity = newEntity;
	}

	void SceneHierarchyPanel::DestroyEntity(Entity entity)
	{
		m_SelectedEntity = {};
		m_Context->DestroyEntity(entity);
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		EntityHierarchyPanel();
		EntityPropertiesPanel();
	}

	void SceneHierarchyPanel::EntityHierarchyPanel()
	{
		OGN_PROFILER_UI();

		ImGui::Begin("Hierarchy");

		if (!m_Context)
		{
			ImGui::End();
			return;
		}

		IsSceneHierarchyFocused = ImGui::IsWindowFocused();

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 0.5f, 2.0f });
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void *)typeid(EditorLayer::Get().m_ActiveScene).hash_code(), treeNodeFlags, EditorLayer::Get().m_ActiveScene->GetName().c_str());
		ImGui::PopStyleVar();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_SOURCE_ITEM"))
			{
				OGN_CORE_ASSERT(payload->DataSize == sizeof(Entity), "WRONG ENTITY ITEM");
				Entity src { *static_cast<entt::entity *>(payload->Data), m_Context.get() };
				if (src.HasParent())
					auto &srcIDC = src.GetComponent<IDComponent>().Parent = 0;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine(ImGui::GetWindowWidth() - 24.0f);
		ImTextureID texId = reinterpret_cast<ImTextureID>(EditorLayer::Get().m_UITextures.at("plus")->GetRendererID());
		if (ImGui::ImageButton(texId, ImVec2(14.0f, 14.0f)))
			ImGui::OpenPopup("CreateEntity");

		if (ImGui::BeginPopup("CreateEntity"))
		{
			if (ImGui::MenuItem("Empty"))
				EntityManager::CreateEntity("Empty", m_Context.get());

			if (ImGui::BeginMenu("2D"))
			{
				if (ImGui::MenuItem("Sprite"))
					EntityManager::CreateSprite("Sprite", m_Context.get());
				if (ImGui::MenuItem("Circle"))
					EntityManager::CreateCircle("Circle", m_Context.get());
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("3D"))
			{
				if (ImGui::MenuItem("Empty Mesh"))
					EntityManager::CreateMesh("Empty Mesh", m_Context.get());
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Camera"))
				EntityManager::CreateCamera("Camera", m_Context.get());
			if (ImGui::MenuItem("Lighting"))
				EntityManager::CreateLighting("Lighting", m_Context.get());
			ImGui::EndPopup();
		}

		if (open)
		{
			for (auto e : m_Context->m_EntityStorage)
				DrawEntityNode({ e.second, m_Context.get() });
			ImGui::TreePop();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

		if (!m_Context->IsRunning())
		{
			if (ImGui::BeginPopupContextWindow(nullptr, 1))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.f, 20.f));
				EntityContextMenu();
				ImGui::PopStyleVar();

				ImGui::EndPopup();
			}
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}

	void SceneHierarchyPanel::EntityPropertiesPanel()
	{
		ImGui::Begin("Properties");
		IsScenePropertiesFocused = ImGui::IsWindowFocused();
		if (m_SelectedEntity)
			DrawComponents(m_SelectedEntity);
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity, int index)
	{
		OGN_PROFILER_UI();

		auto valid = m_Context->m_Registry.valid(entity);
		if (!valid || (entity.HasParent() && index == 0))
			return;

		ImGuiTreeNodeFlags flags = (m_SelectedEntity == entity ? ImGuiTreeNodeFlags_Selected : 0)
			| ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		ImVec4 headerActive = entity.HasComponent<UIComponent>() ? ImVec4(0.1f, 0.1f, 0.3f, 1.0f) : ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		ImVec4 headerHovered = entity.HasComponent<UIComponent>() ? ImVec4(0.3f, 0.3f, 0.7f, 1.0f) : ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

		if (m_SelectedEntity == entity)
			headerActive = entity.HasComponent<UIComponent>() ? ImVec4(0.3f, 0.3f, 0.9f, 1.0f) : ImVec4(0.47f, 0.47f, 0.47f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_Header, headerActive);
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, headerHovered);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 0.5f, 2.0f });
		bool node_open = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, entity.GetTag().c_str());
		ImGui::PopStyleVar();
		ImGui::PopStyleColor(2);

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
		{
			if (entity.HasComponent<UIComponent>())
			{
				auto &ui = entity.GetComponent<UIComponent>();
				UIEditor::Get()->SetActive(&ui);
			}
		}

		bool isDeleting = false;
		if (!m_Context->IsRunning())
		{
			if (ImGui::BeginPopupContextItem())
			{
				Entity e = EntityContextMenu();
				if (e.GetScene())
				{
					EntityManager::AddChild(entity, e, m_Context.get());
					m_SelectedEntity = e;
				}

				if (ImGui::MenuItem("Delete"))
				{
					DestroyEntity(entity);
					isDeleting = true;
				}

				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("ENTITY_SOURCE_ITEM", &entity, sizeof(Entity));
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ENTITY_SOURCE_ITEM"))
				{
					OGN_CORE_ASSERT(payload->DataSize == sizeof(Entity), "WRONG ENTITY ITEM");
					Entity src { *static_cast<entt::entity *>(payload->Data), m_Context.get() };
					// the current 'entity' is the target (new parent for src)
					EntityManager::AddChild(entity, src, m_Context.get());
				}
				ImGui::EndDragDropTarget();
			}
		}

		if (ImGui::IsItemHovered())
		{
			if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
				m_SelectedEntity = entity;
		}

		if (node_open)
		{
			if (!isDeleting)
			{
				for (auto e : m_Context->m_EntityStorage)
				{
					valid = m_Context->m_Registry.valid(e.second);
					if (!valid)
						break;

					Entity ent = { e.second, m_Context.get() };
					if (ent.GetComponent<IDComponent>().Parent == entity.GetUUID())
						DrawEntityNode({ e.second, m_Context.get() }, index + 1);
				}
			}
			ImGui::TreePop();
		}


		if (!isDeleting)
		{
			ImGui::PushID((void *)(uint64_t)(uint32_t)entity);
			auto &tc = entity.GetComponent<TransformComponent>();
			ImTextureID texId = reinterpret_cast<ImTextureID>(EditorLayer::Get().m_UITextures.at("eyes_open")->GetRendererID());
			if (!tc.Visible)
				texId = reinterpret_cast<ImTextureID>(EditorLayer::Get().m_UITextures.at("eyes_closed")->GetRendererID());
			ImGui::SameLine(ImGui::GetWindowWidth() - 24.0f);
			if (ImGui::ImageButton(texId, ImVec2(14.0f, 14.0f)))
				tc.Visible = !tc.Visible;
			ImGui::PopID();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		OGN_PROFILER_UI();

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
			std::string search = "Search Component";
			char searchBuffer[256];
			strcpy_s(searchBuffer, sizeof(searchBuffer), search.c_str());
			if (ImGui::InputText("##SearchComponent", searchBuffer, sizeof(searchBuffer)))
				search = std::string(searchBuffer);

			DisplayAddComponentEntry<ScriptComponent>("SCRIPT");
			DisplayAddComponentEntry<CameraComponent>("CAMERA");
			DisplayAddComponentEntry<AudioComponent>("AUDIO");
			DisplayAddComponentEntry<AudioListenerComponent>("AUDIO LISTENER");
			DisplayAddComponentEntry<UIComponent>("UI");
			DisplayAddComponentEntry<SpriteRenderer2DComponent>("2D SPRITE RENDERER 2D");
			DisplayAddComponentEntry<BoxCollider2DComponent>("2D BOX COLLIDER");
			DisplayAddComponentEntry<CircleCollider2DComponent>("2D CIRCLE COLLIDER");
			DisplayAddComponentEntry<RevoluteJoint2DComponent>("2D REVOLUTE JOINT");
			DisplayAddComponentEntry<SpriteAnimationComponent>("2D SPRITE ANIMATION");
			DisplayAddComponentEntry<CircleRendererComponent>("2D CIRCLE RENDERER 2D");
			if (!m_SelectedEntity.HasComponent<Rigidbody2DComponent>())
			{
				DisplayAddComponentEntry<Rigidbody2DComponent>("2D RIGIDBODY");
			}
			DisplayAddComponentEntry<StaticMeshComponent>("STATIC MESH COMPONENT");
			DisplayAddComponentEntry<ParticleComponent>("PARTICLE");
			DisplayAddComponentEntry<TextComponent>("TEXT COMPONENT");
			if (DisplayAddComponentEntry<LightComponent>("LIGHTING"))
			{
				auto &lc = m_SelectedEntity.GetComponent<LightComponent>();
				if (!lc.Light)
					lc.Light = Lighting::Create(LightingType::Directional);
			}
			DisplayAddComponentEntry<RigidbodyComponent>("RIGIDBODY");
			DisplayAddComponentEntry<BoxColliderComponent>("BOX COLLIDER");
			DisplayAddComponentEntry<SphereColliderComponent>("SPHERE COLLIDER");
			DisplayAddComponentEntry<CapsuleColliderComponent>("CAPSULE COLLIDER");

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("TRANSFORM", entity, [&](auto& component)
		{
			UI::DrawVec3Control("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			UI::DrawVec3Control("Rotation", rotation, 1.0f);
			component.Rotation = glm::radians(rotation);
			UI::DrawVec3Control("Scale", component.Scale, 0.01f, 1.0f);
		});

		DrawComponent<StaticMeshComponent>("STATIC MESH", entity, [](auto& component)
			{
				OGN_PROFILER_UI();

				std::string lable = "None";

				if (component.Handle != 0)
				{
					if (AssetManager::IsAssetHandleValid(component.Handle) && AssetManager::GetAssetType(component.Handle) == AssetType::StaticMesh)
					{
						const AssetMetadata &metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(component.Handle);
						lable = metadata.Filepath.filename().string();
					}
					else
					{
						lable = "Invalid";
					}
				}

				ImVec2 buttonSize = ImVec2(100.0f, 25.0f);
				// Model Button
				ImGui::Button(lable.c_str(), buttonSize);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						AssetHandle handle = *static_cast<AssetHandle*>(payload->Data);
						if (AssetManager::GetAssetType(handle) == AssetType::StaticMesh)
						{
							component.Handle = handle;
						}
						else
						{
							OGN_CORE_WARN("Wrong asset type!");
						}
					}
					ImGui::EndDragDropTarget();
				}

				const ImVec2 xLabelSize = ImGui::CalcTextSize("X");
				const float xSize = xLabelSize.y + ImGui::GetStyle().FramePadding.y * 2.0f;

				if (component.Handle != 0)
				{
					ImGui::SameLine();
					ImGui::PushID("model_delete");
					if (ImGui::Button("X", ImVec2(xSize, buttonSize.y)))
						component.Handle = 0;
					ImGui::PopID();

					std::shared_ptr<Model> &model = AssetManager::GetAsset<Model>(component.Handle);

					if (component.MaterialHandle != 0)
					{
						if (AssetManager::IsAssetHandleValid(component.MaterialHandle) && AssetManager::GetAssetType(component.MaterialHandle) == AssetType::Material)
						{
							const AssetMetadata &metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(component.MaterialHandle);
							lable = metadata.Filepath.filename().string();
						}
						else
						{
							lable = "Default";
						}
					}

					// Material Button
					ImGui::Button(lable.c_str(), buttonSize);

					if(model->GetMaterial())
					if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
					{
						EditorLayer::Get().m_MaterialEditor.SetSelectedMaterial(model->GetMaterial()->Handle);
					}

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
							if (AssetManager::GetAssetType(handle) == AssetType::Material)
							{
								std::shared_ptr<Material> mat = AssetManager::GetAsset<Material>(handle);
								model->SetMaterial(mat);
								component.MaterialHandle = handle;
							}
							else
							{
								OGN_CORE_WARN("Wrong asset type!");
							}
						}
						ImGui::EndDragDropTarget();
					}

					if (component.MaterialHandle != 0)
					{
						ImGui::SameLine();
						ImGui::PushID("material_delete");
						if (ImGui::Button("X", ImVec2(xSize, buttonSize.y)))
						{
							component.MaterialHandle = 0;
							model->RemoveMaterial();
						}
						ImGui::PopID();
					}
				}
			});

			DrawComponent<UIComponent>("UI", entity, [](UIComponent &component)
			{
				if(ImGui::Button("Edit"))
				{
					UIEditor::Get()->SetActive(&component);
				}
			});

		DrawComponent<CameraComponent>("CAMERA", entity, [](auto& component)
		{
			OGN_PROFILER_UI();

			auto& camera = component.Camera;
			UI::DrawCheckbox("Primary", &component.Primary);

			const char* projectionType[2] = { "Perspective", "Orthographic" };
			const char* currentProjectionType = projectionType[static_cast<int>(camera.GetProjectionType())];

			bool isSelected = false;
			if (ImGui::BeginCombo("Projection", currentProjectionType))
			{
				for (int i = 0; i < 2; i++)
				{
					isSelected = currentProjectionType == projectionType[i];
					if (ImGui::Selectable(projectionType[i], isSelected))
					{
						currentProjectionType = projectionType[i];
						component.Camera.SetProjectionType(static_cast<ProjectionType>(i));
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

		const char* aspectRatioType[5] = { "Free", "16/10", "16/9", "21/9", "4/3" };
		const char* currentAspectRatioType = aspectRatioType[static_cast<int>(camera.GetAspectRatioType())];
		
		if (ImGui::BeginCombo("Aspect Ratio", currentAspectRatioType))
		{
			for (int i = 0; i < 5; i++)
			{
				isSelected = currentAspectRatioType == aspectRatioType[i];
				if (ImGui::Selectable(aspectRatioType[i], isSelected))
				{
					currentAspectRatioType = aspectRatioType[i];
					camera.SetAspectRatioType(static_cast<SceneCamera::AspectRatioType>(i));
					EditorLayer::Get().m_UIEditor->OnCamViewportSizeChange();
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (component.Camera.GetProjectionType() == ProjectionType::Perspective)
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

		if (component.Camera.GetProjectionType() == ProjectionType::Orthographic)
		{
			float orthoScale = camera.GetOrthographicScale();
			camera.SetOrthographicScale(orthoScale);

			if (ImGui::DragFloat("Ortho Size", &orthoScale, 0.1f, 1.0f, 100.0f))
				camera.SetOrthographicScale(orthoScale);

			float orthoNearClip = camera.GetOrthographicNearClip();
			if (ImGui::DragFloat("Near Clip", &orthoNearClip, 0.1f, -1.0f, 10.0f))
				camera.SetOrthographicNearClip(orthoNearClip);

			float orthoFarClip = camera.GetOrthographicFarClip();
			if (ImGui::DragFloat("Far Clip", &orthoFarClip, 0.1f, 10.0f, 100.0f))
				camera.SetOrthographicFarClip(orthoFarClip);
		}});

		DrawComponent<SpriteAnimationComponent>("SPRITE ANIMATION", entity, [](auto& component)
		{
				OGN_PROFILER_UI();

			for (auto anim : component.State->GetStateStorage())
			{
				ImGui::Text(anim.c_str());
			}
		});

		DrawComponent<RigidbodyComponent>("RIGIDBODY", entity, [](auto& component)
		{
				OGN_PROFILER_UI();

				UI::DrawCheckbox("UseGravity", &component.UseGravity);
				UI::DrawCheckbox("Rotate X", &component.RotateX);
				UI::DrawCheckbox("Rotate Y", &component.RotateY);
				UI::DrawCheckbox("Rotate Z", &component.RotateZ);

				UI::DrawCheckbox("##Kinematic", &component.Kinematic);
				UI::DrawVecControl("Mass", &component.Mass, 0.05f, 0.0f, 1000.0f, 0.0f);
				UI::DrawVec3Control("Center Mass", component.CenterMassPosition);

		});

		DrawComponent<BoxColliderComponent>("BOX COLLIDER", entity, [](auto& component)
		{
				OGN_PROFILER_UI();

				UI::DrawVec3Control("Offset", component.Offset, 0.025f, 0.0f);
				UI::DrawVec3Control("Size", component.Size, 0.025f, 0.5f);
				UI::DrawVecControl("StaticFriction", &component.StaticFriction, 0.025f, 0.0f, 1000.0f, 0.5f);
				UI::DrawVecControl("DynamicFriction", &component.DynamicFriction, 0.025f, 0.0f, 1000.0f, 0.5f);
				UI::DrawVecControl("Restitution", &component.Restitution, 0.025f, 0.0f, 1000.0f, 0.0f);
		});

		DrawComponent<SphereColliderComponent>("SPHERE COLLIDER", entity, [](auto& component)
			{
				OGN_PROFILER_UI();

				UI::DrawVec3Control("Offset", component.Offset, 0.025f, 0.0f);
				UI::DrawVecControl("Radius", &component.Radius, 0.025f, 0.0f, 10.0f, 1.0f);
				UI::DrawVecControl("StaticFriction", &component.StaticFriction, 0.025f, 0.0f, 1000.0f, 0.5f);
				UI::DrawVecControl("DynamicFriction", &component.DynamicFriction, 0.025f, 0.0f, 1000.0f, 0.5f);
				UI::DrawVecControl("Restitution", &component.Restitution, 0.025f, 0.0f, 1000.0f, 0.0f);
			});

		DrawComponent<CapsuleColliderComponent>("CAPSULE COLLIDER", entity, [](auto& component)
			{
				OGN_PROFILER_UI();

				UI::DrawCheckbox("Horizontal", &component.Horizontal);
				UI::DrawVec3Control("Offset", component.Offset, 0.01f, 0.0f);
				UI::DrawVecControl("Radius", &component.Radius, 0.01f, 0.0f, 1000.0f, 0.5f);
				UI::DrawVecControl("Height", &component.Height, 0.01f, 0.0f, 1000.0f, 1.0f);
				UI::DrawVecControl("StaticFriction", &component.StaticFriction, 0.025f, 0.0f, 1000.0f, 0.5f);
				UI::DrawVecControl("DynamicFriction", &component.DynamicFriction, 0.025f, 0.0f, 1000.0f, 0.5f);
				UI::DrawVecControl("Restitution", &component.Restitution, 0.025f, 0.0f, 1000.0f, 0.0f);
			});

		DrawComponent<AudioComponent>("AUDIO SOURCE", entity, [entity, scene = m_Context](auto& component)
			{
				OGN_PROFILER_UI();

				std::string label = "None";

				bool isAudioValid = false;
				ImGui::Text("Audio Source");
				ImGui::SameLine();
				if (component.Audio != 0)
				{
					if (AssetManager::IsAssetHandleValid(component.Audio) && AssetManager::GetAssetType(component.Audio) == AssetType::Audio)
					{
						const AssetMetadata& metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(component.Audio);
						label = metadata.Filepath.filename().string();
						isAudioValid = true;
					}
					else
					{
						label = "Invalid";
					}
				}

				ImVec2 buttonLabelSize = ImGui::CalcTextSize(label.c_str());
				buttonLabelSize.x += 20.0f;
				const auto buttonLabelWidth = glm::max<float>(100.0f, buttonLabelSize.x);

				ImGui::Button(label.c_str(), ImVec2(buttonLabelWidth, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						AssetHandle handle = *static_cast<AssetHandle*>(payload->Data);
						if (AssetManager::GetAssetType(handle) == AssetType::Audio)
						{
							component.Audio = handle;
							const AssetMetadata& metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(component.Audio);
							component.Name = metadata.Filepath.filename().string();
						}
						else
						{
							OGN_CORE_WARN("Wrong asset type!");
						}
					}
					ImGui::EndDragDropTarget();
				}

				if (isAudioValid == false)
					return;

				std::shared_ptr<AudioSource> audio = AssetManager::GetAsset<AudioSource>(component.Audio);

				if (audio->IsLoaded)
				{
					auto &name = component.Name;
					char buffer[256];
					ImGui::Text("Name");
					ImGui::SameLine();
					strcpy_s(buffer, sizeof(buffer), name.c_str());
					if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
					{
						name = std::string(buffer);
						audio->SetName(name.c_str());
					}

					UI::DrawCheckbox("Play At Start", &component.PlayAtStart);
					UI::DrawCheckbox("Looping", &component.Looping);
					UI::DrawVecControl("Volume", &component.Volume, 0.025f, 0.0f, 1.0f, 1.0f);
					UI::DrawVecControl("Pitch", &component.Pitch, 0.025f, 0.0f, 1.0f, 1.0f);
					UI::DrawVecControl("Panning", &component.Panning, 0.025f, -1.0f, 1.0f, 0.0f);
					float sizeX = ImGui::GetContentRegionAvail().x;
					if (ImGui::Button("Play", { sizeX, 0.0f })) audio->Play();
					if (ImGui::Button("Pause", { sizeX, 0.0f })) audio->Pause();
					if (ImGui::Button("Stop", { sizeX, 0.0f })) audio->Stop();
					ImGui::Separator();
					UI::DrawCheckbox("Spatialize", &component.Spatializing);

					if (component.Spatializing)
					{
						UI::DrawVecControl("Min Distance", &component.MinDistance, 0.1f, 0.0f, 10000.0f, 0.0f);
						UI::DrawVecControl("Max Distance", &component.MaxDistance, 0.1f, 0.0f, 10000.0f, 0.0f);
					}
				}
			});

		DrawComponent<TextComponent>("TEXT", entity, [](auto& component) 
			{
				OGN_PROFILER_UI();

				ImGui::Button("DROP FONT", ImVec2(80.0f, 30.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						AssetHandle handle = *static_cast<AssetHandle *>(payload->Data);
						if (AssetManager::GetAssetType(handle) == AssetType::Font)
							component.FontHandle = handle;
					}
				}

				if (component.FontHandle)
				{
					ImGui::SameLine();
					if (ImGui::Button("X"))
						component.FontHandle = 0;
				}
				
				if (component.FontHandle != 0)
				{
					ImGui::InputTextMultiline("Text String", &component.TextString);
					ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
					UI::DrawVecControl("Kerning", &component.Kerning, 0.01f);
					UI::DrawVecControl("Line Spacing", &component.LineSpacing, 0.01f);
					UI::DrawCheckbox("Screen Space", &component.ScreenSpace);
				}

			});

		DrawComponent<ParticleComponent>("PARTICLE", entity, [](auto& component)
			{
				OGN_PROFILER_UI();

				float columnWidth = 100.0f;

				ImGui::ColorEdit4("Color Begin", glm::value_ptr(component.ColorBegin));
				ImGui::ColorEdit4("Color End", glm::value_ptr(component.ColorEnd));
				UI::DrawVec3Control("Velocity", component.Velocity, 0.01f, 0.0f, columnWidth);
				UI::DrawVec3Control("Velocity Variation", component.VelocityVariation, 0.01f, 0.0f, columnWidth);
				UI::DrawVec3Control("Rotation", component.Rotation, 0.01f, 0.0f, columnWidth);

				UI::DrawVecControl("Size Begin", &component.SizeBegin, 0.01f, 0.0f, 1000.0f, 0.5f, columnWidth);
				UI::DrawVecControl("Size End", &component.SizeEnd, 0.01f, 0.0f, 1000.0f, 0.0f, columnWidth);
				UI::DrawVecControl("Size Variation", &component.SizeVariation, 0.1f, 0.0f, 1000.0f, 0.3f, columnWidth);
				UI::DrawVecControl("Z Axis", &component.ZAxis, 0.1f, -1000.0f, 1000.0f, 0.0f, columnWidth);
				UI::DrawVecControl("Life Time", &component.LifeTime, 0.01f, 0.0f, 1000.0f, 1.0f, columnWidth);
			});

		DrawComponent<SpriteRenderer2DComponent>("SPRITE RENDERER 2D", entity, [](auto& component)
			{
				OGN_PROFILER_UI();

				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				std::string label = "None";
				ImGui::Text("Texture");
				ImGui::SameLine();
				if (component.Texture != 0)
				{
					if (AssetManager::IsAssetHandleValid(component.Texture) && AssetManager::GetAssetType(component.Texture) == AssetType::Texture)
					{
						const AssetMetadata& metadata = Project::GetActive()->GetEditorAssetManager()->GetMetadata(component.Texture);
						label = metadata.Filepath.filename().string();
					}
					else
					{
						label = "Invalid";
					}
				}

				ImVec2 buttonLabelSize = ImGui::CalcTextSize(label.c_str());
				buttonLabelSize.x += 20.0f;
				const auto buttonLabelWidth = glm::max<float>(100.0f, buttonLabelSize.x);

				ImGui::Button(label.c_str(), ImVec2(buttonLabelWidth, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						AssetHandle handle = *static_cast<AssetHandle*>(payload->Data);
						if (AssetManager::GetAssetType(handle) == AssetType::Texture)
						{
							component.Texture = handle;
							component.Min = glm::vec2(0.0f);
							component.Max = glm::vec2(1.0f);
						}
						else
							OGN_CORE_WARN("Wrong asset type!");
					}
					else if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SPRITESHEET_ITEM"))
					{
						SpriteSheetData data = *static_cast<SpriteSheetData *>(payload->Data);
						component.Texture = data.TextureHandle;
						component.Min = data.Min;
						component.Max = data.Max;
					}
					ImGui::EndDragDropTarget();
				}

				if (component.Texture != 0)
				{
					ImGui::SameLine();
					const ImVec2 xLabelSize = ImGui::CalcTextSize("X");
					const float buttonSize = xLabelSize.y + ImGui::GetStyle().FramePadding.y * 2.0f;
					if (ImGui::Button("X", ImVec2(buttonSize, buttonSize)))
					{
						component.Texture = 0;
						component.Min = glm::vec2(0.0f);
						component.Max = glm::vec2(1.0f);
					}

					UI::DrawVec2Control("Tilling Factor", component.TillingFactor, 0.025f, 1.0f);
					ImGui::Text("Flip");
					ImGui::SameLine();
					ImGui::PushID(1);
					UI::DrawCheckbox("X", &component.FlipX);
					ImGui::PopID();
					ImGui::SameLine();
					UI::DrawCheckbox("Y", &component.FlipY);
				}
			});

		DrawComponent<LightComponent>("LIGHTING", entity, [](auto& component)
			{
				OGN_PROFILER_UI();

				const char* lightTypeString[3] = { "Spot", "Point", "Directional" };
				const char* currentLightTypeString = lightTypeString[static_cast<int>(component.Light->Type)];

				if (ImGui::BeginCombo("Type", currentLightTypeString))
				{
					for (int i = 0; i < 3; i++)
					{
						bool isSelected = currentLightTypeString == lightTypeString[i];
						if (ImGui::Selectable(lightTypeString[i], isSelected))
						{
							currentLightTypeString = lightTypeString[i];
							component.Light->Type = static_cast<LightingType>(i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				switch (component.Light->Type)
				{
					case LightingType::Directional:
					{
						ImGui::ColorEdit3("Color", glm::value_ptr(component.Light->m_DirLightData.Color));
						UI::DrawVecControl("Strength", &component.Light->m_DirLightData.Strength, 0.01f, 0.0f, 100.0f);
						UI::DrawVecControl("Diffuse", &component.Light->m_DirLightData.Diffuse, 0.01f, 0.0f, 1.0f, 1.0f);
						UI::DrawVecControl("Specular", &component.Light->m_DirLightData.Specular, 0.01f, 0.0f, 1.0f, 1.0f);
						UI::DrawVecControl("Far", &component.Light->GetShadow()->Far, 1.0f, -1000.0f, 1000.0f, 50.0f);
						UI::DrawVecControl("Near", &component.Light->GetShadow()->Near, 1.0f, -1000.0f, 1000.0f, -10.0f);
						UI::DrawVecControl("Size", &component.Light->GetShadow()->Size, 1.0f, -1000.0f, 1000.0f, 50.0f);

						if (component.Light->GetShadow()->GetFramebuffer())
						{
							uint32_t texture = component.Light->GetShadow()->GetFramebuffer()->GetDepthAttachmentRendererID();
							float size = ImGui::GetContentRegionAvail().x;
							ImGui::Image(reinterpret_cast<ImTextureID>(texture), ImVec2(size, size), ImVec2(0, 1), ImVec2(1, 0));
						}
						break;	
					}
#if 0
					case LightingType::Spot:
					{
						ImGui::ColorEdit3("Color", glm::value_ptr(component.Light->Color));

						float angle = glm::degrees(component.Light->InnerConeAngle);
						ImGui::DragFloat("Inner Cone", &angle);
						component.Light->InnerConeAngle = glm::radians(angle);

						angle = glm::degrees(component.Light->OuterConeAngle);
						ImGui::DragFloat("Outer Cone", &angle);
						component.Light->OuterConeAngle = glm::radians(angle);

						ImGui::DragFloat("Exponent", &component.Light->Exponent, 0.01f, 0.0f, 1.0f);
						break;
					}

					case LightingType::Point:
					{
						ImGui::ColorEdit3("Color", glm::value_ptr(component.Light->Color));
						UI::DrawVecControl("Ambient", &component.Light->Ambient, 0.01f, 0.0f);
						UI::DrawVecControl("Specular", &component.Light->Specular, 0.01f, 0.0f);
						UI::DrawVecControl("Intensity", &component.Light->Intensity, 0.01f, 0.0f);
						UI::DrawVecControl("Size", &component.Light->SpreadSize, 0.1f, 0.1f, 10000.0f);
						break;
					}
#endif
				}
				
			});

		DrawComponent<CircleRendererComponent>("CIRCLE RENDERER", entity, [](auto& component)
			{
				OGN_PROFILER_UI();

				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.025f, 0.0f, 1.0f);
			});

		DrawComponent<Rigidbody2DComponent>("RIGID BODY 2D", entity, [](auto& component)
		{
				OGN_PROFILER_UI();

			UI::DrawCheckbox("Enabled", &component.Enabled);
			const char* bodyTypeString[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeString[static_cast<int>(component.Type)];

			if (ImGui::BeginCombo("Body Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeString[i];
					if (ImGui::Selectable(bodyTypeString[i], isSelected))
					{
						currentBodyTypeString = bodyTypeString[i];
						component.Type = static_cast<Rigidbody2DComponent::BodyType>(i);
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			UI::DrawVecControl("Gravity Scale", &component.GravityScale, 0.01f);
			UI::DrawVecControl("Rotational Inertia", &component.RotationalInertia, 0.01f);
			UI::DrawVecControl("Linear Damping", &component.LinearDamping, 0.025f, 0.0f, 1000.0f);
			UI::DrawVecControl("Angular Damping", &component.AngularDamping, 0.025f, 0.0f, 1000.0f);
			UI::DrawVecControl("Mass", &component.Mass, 0.01f);
			UI::DrawVec2Control("Mass Center", component.MassCenter, 0.01f);

			ImGui::Text("Freeze Position");
			ImGui::SameLine();
			UI::DrawCheckbox("X", &component.FreezePositionX);
			ImGui::SameLine();
			UI::DrawCheckbox("Y", &component.FreezePositionY);

			UI::DrawCheckbox("Fixed Rotation", &component.FixedRotation);
			UI::DrawCheckbox("Awake", &component.Awake);
			ImGui::SameLine();
			UI::DrawCheckbox("Allow Sleeping", &component.AllowSleeping);
			UI::DrawCheckbox("Bullet", &component.Bullet);

			});

		DrawComponent<BoxCollider2DComponent>("BOX COLLIDER 2D", entity, [](auto& component)
			{
				OGN_PROFILER_UI();

				ImGui::DragInt("Group Index", &component.Group, 1.0f, -1, 16, "Group Index %d");

				UI::DrawVec2Control("Offset", component.Offset, 0.01f, 0.0f);
				glm::vec2 size = component.Size * glm::vec2(2.0f);
				UI::DrawVec2Control("Size", size, 0.01f, 0.5f);
				component.Size = size / glm::vec2(2.0f);

				float width = 118.0f;
				b2Fixture* fixture = static_cast<b2Fixture*>(component.RuntimeFixture);
				if (UI::DrawVecControl("Density", &component.Density, 0.01f, 0.0f, 100.0f, 1.0f, width))
				{
					if(fixture) fixture->SetDensity(component.Density);
				}
				if(UI::DrawVecControl("Friction", &component.Friction, 0.02f, 0.0f, 100.0f, 0.5f, width))
				{
					if(fixture) fixture->SetFriction(component.Friction);
				}
				if(UI::DrawVecControl("Restitution", &component.Restitution, 0.01f, 0.0f, 100.0f, 0.5f, width))
				{
					if(fixture) fixture->SetRestitution(component.Restitution);
				}
				if(UI::DrawVecControl("Threshold", &component.RestitutionThreshold, 0.01f, 0.0f, 100.0f, 0.0f, width))
				{
					if(fixture) fixture->SetRestitutionThreshold(component.RestitutionThreshold);
				}
			});

		DrawComponent<CircleCollider2DComponent>("CIRCLE COLLIDER 2D", entity, [](auto& component)
			{
				OGN_PROFILER_UI();

				ImGui::DragInt("Group Index", &component.Group, 1.0f, -1, 16, "Group Index %d");

				UI::DrawVec2Control("Offset", component.Offset, 0.01f, 0.0f);
				UI::DrawVecControl("Radius", &component.Radius, 0.01f, 0.5f);

				float width = 118.0f;
				b2Fixture* fixture = static_cast<b2Fixture*>(component.RuntimeFixture);
				if (UI::DrawVecControl("Density", &component.Density, 0.01f, 0.0f, 100.0f, 1.0f, width))
				{
					if (fixture) fixture->SetDensity(component.Density);
				}
				if (UI::DrawVecControl("Friction", &component.Friction, 0.02f, 0.0f, 100.0f, 0.5f, width))
				{
					if (fixture) fixture->SetFriction(component.Friction);
				}
				if (UI::DrawVecControl("Restitution", &component.Restitution, 0.01f, 0.0f, 100.0f, 0.5f, width))
				{
					if (fixture) fixture->SetRestitution(component.Restitution);
				}
				if (UI::DrawVecControl("Threshold", &component.RestitutionThreshold, 0.01f, 0.0f, 100.0f, 0.0f, width))
				{
					if (fixture) fixture->SetRestitutionThreshold(component.RestitutionThreshold);
				}
			});

		DrawComponent<RevoluteJoint2DComponent>("REVOLUTE JOINT 2D", entity, [&](auto& component)
			{
				OGN_PROFILER_UI();

				std::string label = "Connected Body";

				if (component.ConnectedBodyID != 0)
					label = "Connected";

				ImGui::Button(label.c_str());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_SOURCE_ITEM"))
					{
						OGN_CORE_ASSERT(payload->DataSize == sizeof(Entity), "WRONG ENTITY ITEM");
						Entity src{ *static_cast<entt::entity*>(payload->Data), m_Context.get() };
						if (component.ConnectedBodyID == 0)
						{
							UUID uuid = src.GetUUID();
							component.ConnectedBodyID = uuid;
						}
					}
					ImGui::EndDragDropTarget();
				}

				if (component.ConnectedBodyID != 0)
				{
					ImGui::SameLine();
					const ImVec2 xLabelSize = ImGui::CalcTextSize("X");
					const float buttonSize = xLabelSize.y + ImGui::GetStyle().FramePadding.y * 2.0f;
					if (ImGui::Button("X", ImVec2(buttonSize, buttonSize)))
					{
						component.ConnectedBodyID = 0;
					}
				}

				b2RevoluteJoint* joint = static_cast<b2RevoluteJoint*>(component.Joint);
				if (UI::DrawCheckbox("Limit", &component.EnableLimit))
				{
					if (joint)
						joint->EnableLimit(component.EnableLimit);
				}
				UI::DrawVec2Control("Anchor", component.AnchorPoint);
				if (UI::DrawVecControl("Lower Angle", &component.LowerAngle, 0.0f))
				{
					if (joint)
						joint->SetLimits(glm::radians(component.LowerAngle), glm::radians(component.UpperAngle));
				}
				if (UI::DrawVecControl("Upper Angle", &component.UpperAngle, 0.0f))
				{
					if (joint)
						joint->SetLimits(glm::radians(component.LowerAngle), glm::radians(component.UpperAngle));
				}
				if (UI::DrawVecControl("Max Torque", &component.MaxMotorTorque, 0.0f))
				{
					if (joint)
						joint->SetMaxMotorTorque(component.MaxMotorTorque);
				}
				if (UI::DrawCheckbox("Motor", &component.EnableMotor))
				{
					if (joint)
						joint->EnableMotor(component.EnableMotor);
				}
				if (UI::DrawVecControl("Motor Speed", &component.MotorSpeed, 0.0f))
				{
					if (joint)
						joint->SetMotorSpeed(component.MotorSpeed);
				}
			});

		DrawComponent<ScriptComponent>("SCRIPT", entity, [entity, scene = m_Context](auto& component) mutable
			{
				OGN_PROFILER_UI();

				bool scriptClassExist = ScriptEngine::EntityClassExists(component.ClassName);
				bool isSelected = false;

				if (!scriptClassExist)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));

				auto& scriptStorage = ScriptEngine::GetScriptClassStorage();
				std::string currentScriptClasses = component.ClassName;

				// drop-down
				if (ImGui::BeginCombo("Script Class", currentScriptClasses.c_str()))
				{
					for (int i = 0; i < scriptStorage.size(); i++)
					{
						isSelected = currentScriptClasses == scriptStorage[i];
						if (ImGui::Selectable(scriptStorage[i].c_str(), isSelected))
						{
							currentScriptClasses = scriptStorage[i];
							component.ClassName = scriptStorage[i];
						}
						if (isSelected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Detach"))
				{
					component.ClassName = "Detached";
					isSelected = false;
				}

				bool detached = component.ClassName == "Detached";

				// fields
				bool isRunning = scene->IsRunning();
				if (isRunning)
				{
					std::shared_ptr<ScriptInstance> scriptInstance = ScriptEngine::GetEntityScriptInstance(entity.GetUUID());
					if (scriptInstance && !detached)
					{
						auto& fields = scriptInstance->GetScriptClass()->GetFields();

						for (const auto& [name, field] : fields)
						{
							if (field.Type == ScriptFieldType::Float)
							{
								float data = scriptInstance->GetFieldValue<float>(name);
								if (ImGui::DragFloat(name.c_str(), &data, 0.25f))
								{
									scriptInstance->SetFieldValue<float>(name, data);
								}
							}
							if (field.Type == ScriptFieldType::Int)
							{
								int data = scriptInstance->GetFieldValue<int>(name);
								if (ImGui::DragInt(name.c_str(), &data, 1))
								{
									scriptInstance->SetFieldValue<int>(name, data);
								}
							}
							if (field.Type == ScriptFieldType::Vector2)
							{
								glm::vec2 data = scriptInstance->GetFieldValue<glm::vec2>(name);
								if (ImGui::DragFloat2(name.c_str(), glm::value_ptr(data), 0.25f))
								{
									scriptInstance->SetFieldValue<glm::vec2>(name, data);
								}
							}
							if (field.Type == ScriptFieldType::Vector3)
							{
								glm::vec3 data = scriptInstance->GetFieldValue<glm::vec3>(name);
								if (ImGui::DragFloat3(name.c_str(), glm::value_ptr(data), 0.25f))
								{
									scriptInstance->SetFieldValue<glm::vec3>(name, data);
								}
							}
							if (field.Type == ScriptFieldType::Vector4)
							{
								glm::vec4 data = scriptInstance->GetFieldValue<glm::vec4>(name);
								if (ImGui::DragFloat4(name.c_str(), glm::value_ptr(data), 0.25f))
								{
									scriptInstance->SetFieldValue<glm::vec4>(name, data);
								}
							}
						}
					}
				} // !IsRunning
				else
				{
					if (scriptClassExist && !detached)
					{
						std::shared_ptr<ScriptClass> entityClass = ScriptEngine::GetEntityClass(component.ClassName);
						const auto& fields = entityClass->GetFields();
						auto& entityFields = ScriptEngine::GetScriptFieldMap(entity);

						for (const auto& [name, field] : fields)
						{
							if (entityFields.find(name) != entityFields.end())
							{
								ScriptFieldInstance& scriptField = entityFields.at(name);

								if (field.Type == ScriptFieldType::Float)
								{
									float data = scriptField.GetValue<float>();
									if (ImGui::DragFloat(name.c_str(), &data, 0.25f))
									{
										scriptField.SetValue<float>(data);
									}
								}
								if (field.Type == ScriptFieldType::Int)
								{
									int data = scriptField.GetValue<int>();
									if (ImGui::DragInt(name.c_str(), &data, 1))
									{
										scriptField.SetValue<int>(data);
									}
								}
								if (field.Type == ScriptFieldType::Vector2)
								{
									glm::vec2 data = scriptField.GetValue<glm::vec3>();
									if (ImGui::DragFloat2(name.c_str(), glm::value_ptr(data), 0.25f))
									{
										scriptField.SetValue<glm::vec2>(data);
					}
				}
								if (field.Type == ScriptFieldType::Vector3)
								{
									glm::vec3 data = scriptField.GetValue<glm::vec3>();
									if (ImGui::DragFloat3(name.c_str(), glm::value_ptr(data), 0.25f))
									{
										scriptField.SetValue<glm::vec3>(data);
									}
								}
								if (field.Type == ScriptFieldType::Vector4)
								{
									glm::vec4 data = scriptField.GetValue<glm::vec4>();
									if (ImGui::DragFloat4(name.c_str(), glm::value_ptr(data), 0.25f))
									{
										scriptField.SetValue<glm::vec4>(data);
									}
								}
			}
							else
							{
								if (field.Type == ScriptFieldType::Float)
								{
									float data = 0.0f;
									if (ImGui::DragFloat(name.c_str(), &data))
									{
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue<float>(data);
									}
								}

								if (field.Type == ScriptFieldType::Int)
								{
									int data = 0;
									if (ImGui::DragInt(name.c_str(), &data))
									{
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue<int>(data);
									}
								}

								if (field.Type == ScriptFieldType::Vector2)
								{
									glm::vec2 data(0.0f);
									if (ImGui::DragFloat2(name.c_str(), glm::value_ptr(data)))
									{
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue<glm::vec2>(data);
									}
								}

								if (field.Type == ScriptFieldType::Vector3)
								{
									glm::vec3 data(0.0f);
									if (ImGui::DragFloat3(name.c_str(), glm::value_ptr(data)))
									{
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue<glm::vec3>(data);
									}
								}

								if (field.Type == ScriptFieldType::Vector4)
								{
									glm::vec4 data(0.0f);
									if (ImGui::DragFloat4(name.c_str(), glm::value_ptr(data)))
									{
										ScriptFieldInstance& fieldInstance = entityFields[name];
										fieldInstance.Field = field;
										fieldInstance.SetValue<glm::vec4>(data);
									}
								}
							}
						}
					}
				}

				if (!scriptClassExist)
					ImGui::PopStyleColor();
			});

		DrawComponent<AudioListenerComponent>("AUDIO LISTENER", entity, [](auto& component)
			{
				OGN_PROFILER_UI();

				UI::DrawCheckbox("Enable", &component.Enable);
			});
	}

	Entity SceneHierarchyPanel::EntityContextMenu()
	{
		OGN_PROFILER_UI();

		Entity entity = {};

		if (ImGui::BeginMenu("CREATE"))
		{
			if (ImGui::MenuItem("Empty")) 
				entity = EntityManager::CreateEntity("Empty", m_Context.get());

			if (ImGui::BeginMenu("2D"))
			{
				if (ImGui::MenuItem("Sprite")) 
					entity = EntityManager::CreateSprite("Sprite", m_Context.get());
				if (ImGui::MenuItem("Circle")) 
					entity = EntityManager::CreateCircle("Circle", m_Context.get());
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("3D"))
			{
				if (ImGui::MenuItem("Empty Mesh"))
					entity = EntityManager::CreateMesh("Empty Mesh", m_Context.get());
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Camera"))
				entity = EntityManager::CreateCamera("Camera", m_Context.get());
			if (ImGui::MenuItem("Lighting"))
				entity = EntityManager::CreateLighting("Lighting", m_Context.get());

			ImGui::EndMenu();
		}

		if (entity)
			m_SelectedEntity = entity;

		return entity;
	}

	template<typename T>
	bool SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
		OGN_PROFILER_UI();

		if (ImGui::MenuItem(entryName.c_str()))
		{
			m_SelectedEntity.AddComponent<T>();
			ImGui::CloseCurrentPopup();

			return true;
		}

		return false;
	}

	template<typename T, typename UIFunction>
	void SceneHierarchyPanel::DrawComponent(const std::string &name, Entity entity, UIFunction uiFunction)
	{
		OGN_PROFILER_UI();

		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto &component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2 { 0.5f, 2.0f });
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void *)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvailable.x - 24.0f);
			ImTextureID texId = reinterpret_cast<ImTextureID>(EditorLayer::Get().m_UITextures.at("plus")->GetRendererID());
			if (ImGui::ImageButton(texId, ImVec2(14.0f, 14.0f)))
				ImGui::OpenPopup("Component Settings");

			bool componentRemoved = false;
			if (ImGui::BeginPopup("Component Settings"))
			{
				if (ImGui::MenuItem("Remove component"))
					componentRemoved = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (componentRemoved)
				entity.RemoveComponent<T>();
		}
	}
}
