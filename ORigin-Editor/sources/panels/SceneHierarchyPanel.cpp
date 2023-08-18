// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#include "SceneHierarchyPanel.h"

#include "..\Editor.h"

#include "Origin\Project\Project.h"
#include "Origin\IO\Input.h"
#include "Origin\Renderer\Texture.h"
#include "Origin\Renderer\Shader.h"
#include "Origin\Scene\Component.h"
#include "Origin\Audio\Audio.h"
#include "Origin\Utils\PlatformUtils.h"
#include "Origin\Scripting\ScriptEngine.h"
#include "Origin\Renderer\Renderer.h"

#include <glm\gtc\type_ptr.hpp>
#include <misc\cpp\imgui_stdlib.h>

namespace origin {
	
	SceneHierarchyPanel* SceneHierarchyPanel::s_Instance = nullptr;

	SceneHierarchyPanel::SceneHierarchyPanel(const std::shared_ptr<Scene>& context)
	{
		SetContext(context);
		s_Instance = this;
	}

	Entity SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		return m_SelectedEntity = entity;
	}

	Entity SceneHierarchyPanel::GetSelectedEntity() const
	{
		return m_SelectedEntity;
	}

	void SceneHierarchyPanel::SetContext(const std::shared_ptr<Scene>& context, bool reset)
	{
		// reset scene
		if (reset)
			m_SelectedEntity = {};

		// set new scene
		m_Context = context;

		// recieve selected entity from new scene
		if (!m_SelectedEntity)
			return;

		auto entityID = m_SelectedEntity.GetUUID();
		auto& newEntity = m_Context->GetEntityWithUUID(entityID);

		if (entityID == newEntity.GetUUID())
			m_SelectedEntity = newEntity;
	}

	void SceneHierarchyPanel::DestroyEntity(Entity entity)
	{
		m_Context->DestroyEntity(entity);
		m_SelectedEntity = {};
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
				m_SelectedEntity = {};

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

			if (m_HierarchyMenuActive)
			{
				if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
				{
					if (ImGui::BeginMenu("CREATE"))
					{
						if (ImGui::MenuItem("EMPTY"))
							m_Context->CreateEntity("Empty");

						if (ImGui::MenuItem("MAIN CAMERA"))
						{
							m_Context->CreateCamera("Main Camera");
							m_SelectedEntity.AddComponent<AudioListenerComponent>();
						}

						if (ImGui::MenuItem("CAMERA"))
							m_Context->CreateCamera("Camera");

						// 2D Entity
						if (ImGui::BeginMenu("2D"))
						{
							if (ImGui::MenuItem("Sprite"))
								m_Context->CreateSpriteEntity();
							if (ImGui::MenuItem("Circle"))
								m_Context->CreateCircle("Circle");

							ImGui::EndMenu();
						}

						if (ImGui::BeginMenu("LIGHT"))
						{
							if (ImGui::MenuItem("Point Light"))
								m_Context->CreatePointlight("Point Light");
							if (ImGui::MenuItem("Spot Light"))
								m_Context->CreateSpotLight("Spot Light");
							ImGui::EndMenu();
						}
						if (ImGui::BeginMenu("MESH"))
						{
							if (ImGui::MenuItem("Empty Mesh"))
								m_Context->CreateMesh("Empty Mesh");

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
		ImGui::Begin("PROPERTIES");
		if (m_SelectedEntity)
			DrawComponents(m_SelectedEntity);
		ImGui::End(); // !Properties
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tagComponent = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tagComponent.c_str());

		if (ImGui::IsItemClicked())
		{
			m_SelectedEntity = entity;
		}

		// destroy entity
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				m_Context->DestroyEntity(entity);
				m_SelectedEntity = {};
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
			std::string search = "Search Component";
			char searchBuffer[256];
			strcpy_s(searchBuffer, sizeof(searchBuffer), search.c_str());
			if (ImGui::InputText("##SearchComponent", searchBuffer, sizeof(searchBuffer)))
				search = std::string(searchBuffer);

			DisplayAddComponentEntry<ScriptComponent>("SCRIPT");
			DisplayAddComponentEntry<CameraComponent>("CAMERA");
			if (DisplayAddComponentEntry<AudioComponent>("AUDIO"))
			{
				m_SelectedEntity.GetComponent<AudioComponent>().Audio = Audio::Create();
			}

			DisplayAddComponentEntry<AudioListenerComponent>("AUDIO LISTENER");

			if (DisplayAddComponentEntry<AnimationComponent>("ANIMATION"))
			{
				m_SelectedEntity.GetComponent<AnimationComponent>().Animation = Animation::Create();
			};

			DisplayAddComponentEntry<SpriteRendererComponent>("SPIRTE RENDERER");
			DisplayAddComponentEntry<SpriteRenderer2DComponent>("SPRITE RENDERER 2D");
			DisplayAddComponentEntry<StaticMeshComponent>("STATIC MESH COMPONENT");
			DisplayAddComponentEntry<TextComponent>("TEXT COMPONENT");
			DisplayAddComponentEntry<PointLightComponent>("POINT LIGHT");
			DisplayAddComponentEntry<SpotLightComponent>("SPOT LIGHT");
			DisplayAddComponentEntry<DirectionalLightComponent>("DIRECTIONAL LIGHT");
			DisplayAddComponentEntry<CircleRendererComponent>("CIRCLE RENDERER");
			DisplayAddComponentEntry<Rigidbody2DComponent>("RIGIDBODY 2D");
			DisplayAddComponentEntry<BoxCollider2DComponent>("BOX COLLIDER 2D");
			DisplayAddComponentEntry<Particle2DComponent>("PARTICLE 2D");
			DisplayAddComponentEntry<CircleCollider2DComponent>("CIRLCE COLLIDER 2D");

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("TRANSFORM", entity, [](auto& component)
		{
			DrawVec3Control("Translation", component.Translation);

			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation, 1.0f);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, 0.01f, 1.0f);
		});

		DrawComponent<CameraComponent>("CAMERA", entity, [](auto& component)
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

		DrawComponent<AnimationComponent>("ANIMATION", entity, [](auto& component)
		{
		});

		

		DrawComponent<AudioComponent>("AUDIO SOURCE", entity, [entity, scene = m_Context](auto& component)
			{
				static bool creationWindow = false;

				if (!component.Audio->IsLoaded())
				{
					if (ImGui::Button("Create Audio"))
						creationWindow = true;
				}

				if(creationWindow)
				{
					ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDocking;

					ImGui::Begin("Audio Creation", &creationWindow, winFlags);
					ImGui::Button("Drop Audio");
					
					static std::string filepath;
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path audioPath = Project::GetAssetFileSystemPath(path);
							if (audioPath.extension() == ".wav" || audioPath.extension() == ".mp3" || audioPath.extension() == ".ogg")
							{
								filepath = audioPath.generic_string();
								OGN_CORE_WARN("Audio Component: Drop Audio From {}", filepath);
							}
						}
					}

					ImGui::SameLine();
					ImGui::Text("Path: %s", filepath.c_str());

					std::string& name = component.Name;
					ImGui::Text("Name: "); ImGui::SameLine();
					char buffer[256];
					strcpy_s(buffer, sizeof(buffer), name.c_str());

					if (ImGui::InputText("##audioName", buffer, sizeof(buffer)))
					{
						name = std::string(buffer);
						component.Name = name;
					}

					static bool spatial = false;
					static bool looping = false;
					static float minDistance = 1.0f;
					static float maxDistance = 100.0f;

					ImGui::Text("Spatial");
					ImGui::SameLine();
					ImGui::Checkbox("##Spatial", &spatial);
					ImGui::Text("Looping");
					ImGui::SameLine();
					ImGui::Checkbox("##Looping", &looping);

					if (spatial)
					{
						ImGui::Text("Min Distance");
						ImGui::SameLine();
						ImGui::DragFloat("##MinDistance", &minDistance, 0.1f, 0.0f, 10000.0f);
						ImGui::Text("Max Distance");
						ImGui::SameLine();
						ImGui::DragFloat("##MaxDistance", &maxDistance, 0.1f, 0.0f, 10000.0f);
					}

					static bool valid = false;
					if (ImGui::Button("Create"))
					{
						AudioConfig config;

						config.Name = component.Name;
						config.Spatial = spatial;
						config.Filepath = filepath;
						config.MinDistance = minDistance;
						config.MaxDistance = maxDistance;
						config.Looping = looping;

						component.Spatial = spatial;
						component.Looping = looping;

						valid = (!name.empty() && !filepath.empty());
						if (valid)
						{
							if (component.Audio->IsLoaded())
							{
								component.Audio->Stop();
								component.Audio.reset();

								component.Audio = Audio::Create();
							}

							component.Audio->LoadSource(config);
							OGN_CORE_WARN("Audio Component: Creating {} Audio...", config.Name);
						}
						else
						{
							OGN_CORE_ERROR("Audio Creation: Invalid Audio Creation. Check the Name or Filepath");
						}
					}
					ImGui::End();
				}

				if (component.Audio->IsLoaded())
				{
					ImGui::Text("%s | Spatialization: %s", component.Name.c_str(), component.Spatial ? "On" : "Off");
					ImGui::Separator();

#if 0
					if (ImGui::Button("Insert To Library"))
						AudioEngine::AudioStorageInsert(component.Audio);
					ImGui::SameLine();
					if (ImGui::Button("Delete From Library"))
						AudioEngine::AudioStorageDelete(component.Audio);
					ImGui::SameLine();
					if (ImGui::Button("Open Audio Library"))
						Editor::Get().guiAudioLibraryWindow = true;
#endif
					if (ImGui::Button("Play")) component.Audio->Play();
					ImGui::SameLine();
					if (ImGui::Button("Stop")) component.Audio->Stop();
					ImGui::SameLine();

					ImGui::Checkbox("Play At Start", &component.PlayAtStart);
					ImGui::SameLine();
					if (ImGui::Checkbox("Looping", &component.Looping))
						component.Audio->SetLoop(component.Looping);

					float columnWidth = 100.0f;

					component.Volume = component.Audio->GetGain();
					DrawVecControl("Volume", &component.Volume, 0.01f, 0.0f, 1.0f, 0.0f, columnWidth);

					if(ImGui::Checkbox("Spatialization", &component.Spatial))
						component.Audio->SetSpatial(component.Spatial);

					if (component.Spatial)
					{
						component.MinDistance = component.Audio->GetMinDistance();
						DrawVecControl("Min Distance", &component.MinDistance, 0.1f, 0.0f, 10000.0f, 0.0f, columnWidth);

						component.MaxDistance = component.Audio->GetMaxDistance();
						DrawVecControl("Max Distance", &component.MaxDistance, 0.1f, 0.0f, 10000.0f, 0.0f, columnWidth);
					}

					if (ImGui::Button("Delete"))
					{
						component.Audio->Stop();
						component.Audio.reset();

						component.Audio = Audio::Create();
					}

					ImGui::SameLine();
					if (ImGui::Button("Re-Create"))
					{
						component.Audio->Stop();
						creationWindow = true;
					}
				}
			});

		DrawComponent<ScriptComponent>("SCRIPT", entity, [entity, scene = m_Context](auto& component) mutable
			{
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
							if (field.Type == ScriptFieldType::Vector3)
							{
								glm::vec3 data = scriptInstance->GetFieldValue<glm::vec3>(name);
								if (ImGui::DragFloat3(name.c_str(),	glm::value_ptr(data), 0.25f))
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

		DrawComponent<StaticMeshComponent>("STATIC MESH", entity, [](auto& component)
			{
				float buttonSize[2] = { 85.0f, 25.0f };

				if (!component.Model)
					ImGui::Button("DROP MODEL", ImVec2(buttonSize[0], buttonSize[1]));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;

						// Get the actual Location from Project Assets Directory
						std::filesystem::path modelPath = Project::GetAssetFileSystemPath(path);

						if (modelPath.extension() == ".gltf"
							|| modelPath.extension() == ".fbx"
							|| modelPath.extension() == ".obj")
						{
							std::shared_ptr<Shader> matShader = Renderer::GetGShader("Mesh");

							component.Material = Material::Create("MeshMaterial");
							component.Material->LoadShader(matShader);

							component.Model = Model::Create(modelPath.string(), component.Material);
						}
					}
				}

				if (component.Model)
				{
					if (ImGui::Button("REMOVE", ImVec2(buttonSize[0], buttonSize[1])))
					{
						component.Model.reset();
						component.Material.reset();
					}
				}

				ImGui::SameLine();

				// Open Mesh
				/*
				if (ImGui::Button("OPEN", ImVec2(85.0f, 25.0f)))
				{
					std::filesystem::path path = FileDialogs::OpenFile("Mesh (*.gltf)\0*.gltf\0");
					auto modelPath = Project::GetAssetFileSystemPath(path);

					if (!modelPath.string().empty())
					{
						component.Material.LoadShader("Resources/Shaders/Mesh2.glsl", false);
						component.Model = Model::Create(modelPath.generic_string(), component.Material);
						component.ModelPath = std::filesystem::relative(modelPath, Project::GetAssetDirectory()).generic_string();
					}
				}*/

				if (component.Model)
				{
					if (ImGui::Button("Refresh Shader", ImVec2(buttonSize[0] + 3.0f, buttonSize[1])))
						component.Material->RefreshShader();

					ImGui::Separator();
					ImGui::Text("Material");

					ImGui::ColorEdit4("Color", glm::value_ptr(component.Material->Color));
					ImGui::DragFloat("Shininess", &component.Material->Shininess, 1.0f, 0.0f, 256.0f);

					ImGui::Text("Point Light: %i", component.PointLightCount);
					ImGui::Text("Spot Light : %i", component.SpotLightCount);
				}

			});

		DrawComponent<TextComponent>("TEXT", entity, [](auto& component) 
			{
			if(!component.FontAsset)
				component.FontAsset = Font::GetDefault();
					
				ImGui::Button("DROP FONT", ImVec2(80.0f, 30.0f));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path fontPath = std::filesystem::relative(Project::GetAssetFileSystemPath(path), Project::GetAssetDirectory());
						if (fontPath.extension() == ".ttf" || fontPath.extension() == ".otf")
						{
							if(component.FontAsset) component.FontAsset.reset();
							component.FontAsset = std::make_shared<Font>(fontPath);
						}
					}
				}
				
				ImGui::InputTextMultiline("Text String", &component.TextString);
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Kerning", &component.Kerning, 0.025f);
				ImGui::DragFloat("Line Spacing", &component.LineSpacing, 0.025f);
			});

		DrawComponent<Particle2DComponent>("PARTICLE 2D", entity, [](auto& component)
			{
				float columnWidth = 100.0f;

				ImGui::ColorEdit4("Color Begin", glm::value_ptr(component.ColorBegin));
				ImGui::ColorEdit4("Color End", glm::value_ptr(component.ColorEnd));
				DrawVec2Control("Velocity", component.Velocity, 0.01f, 0.5f, columnWidth);
				DrawVec2Control("Velocity Variation", component.VelocityVariation, 0.01f, 0.0f, columnWidth);

				DrawVecControl("Size Begin", &component.SizeBegin, 0.01f, 0.0f, 1000.0f, 0.5f, columnWidth);
				DrawVecControl("Size End", &component.SizeEnd, 0.01f, 0.0f, 1000.0f, 0.0f, columnWidth);
				DrawVecControl("Size Variation", &component.SizeVariation, 0.1f, 0.0f, 1000.0f, 0.3f, columnWidth);
				DrawVecControl("Z Axis", &component.ZAxis, 0.1f, -1000.0f, 1000.0f, 0.0f, columnWidth);
				DrawVecControl("Life Time", &component.LifeTime, 0.01f, 0.0f, 1000.0f, 1.0f, columnWidth);
			});

		DrawComponent<SpriteRendererComponent>("SPRITE RENDERER", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				if (!component.Texture)
					ImGui::Button("DROP TEXTURE", ImVec2(80.0f, 30.0f));
				else ImGui::ImageButton((ImTextureID)component.Texture->GetRendererID(), ImVec2(80.0f, 80.0f), ImVec2(0, 1), ImVec2(1, 0));

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = Project::GetAssetFileSystemPath(path);
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

		DrawComponent<SpriteRenderer2DComponent>("SPRITE RENDERER 2D", entity, [](auto& component)
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
						std::filesystem::path texturePath = Project::GetAssetFileSystemPath(path);

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
						component.Texture.reset();
						return;
					}

					auto& path = std::filesystem::relative(component.Texture->GetFilepath(), Project::GetAssetDirectory());

					ImGui::Text("Path: %s", path.string().c_str());
					ImGui::DragFloat("Tilling Factor", &component.TillingFactor, 0.1f, 0.0f, 10.0f);
				}
			});

		DrawComponent<DirectionalLightComponent>("DIRECTIONAL LIGHT", entity, [](auto& component)
			{
				DrawVecControl("Ambient", &component.Ambient, 0.01f, 0.0f);
				DrawVecControl("Diffuse", &component.Diffuse, 0.01f, 0.0f);
				DrawVecControl("Specular", &component.Specular, 0.01f, 0.0f);
			});

		DrawComponent<SpotLightComponent>("SPOT LIGHT", entity, [](auto& component)
			{
				ImGui::ColorEdit3("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Ambient", &component.Ambient, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Specular", &component.Specular, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Inner Cone", &component.InnerCone, 0.01f);
				ImGui::DragFloat("Outer cone", &component.OuterCone, 0.01f);
			});

		DrawComponent<PointLightComponent>("POINT LIGHT", entity, [](auto& component)
			{
				ImGui::ColorEdit3("Color", glm::value_ptr(component.Color));
				DrawVecControl("Ambient", &component.Ambient, 0.01f, 0.0f);
				DrawVecControl("Specular", &component.Specular, 0.01f, 0.0f);
			});

		DrawComponent<CircleRendererComponent>("CIRCLE RENDERER", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.025f, 0.0f, 1.0f);
			});

		DrawComponent<Rigidbody2DComponent>("RIGIDBODY 2D", entity, [](auto& component)
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

		DrawComponent<BoxCollider2DComponent>("BOX COLLIDER 2D", entity, [](auto& component)
			{
				DrawVec2Control("Offset", component.Offset, 0.01f, 0.5f);
				DrawVec2Control("Size", component.Size, 0.01f, 0.5f);

				float width = 118.0f;
				DrawVecControl("Density", &component.Density, 0.01f, 1.0f, width);
				DrawVecControl("Friction", &component.Friction, 0.01f, 0.5f, width);
				DrawVecControl("Restitution", &component.Restitution, 0.01f, 0.0f, width);
				DrawVecControl("Restitution Thrs", &component.RestitutionThreshold, 0.01f, 1.0f, width);
			});

		DrawComponent<CircleCollider2DComponent>("CIRCLE COLLIDER 2D", entity, [](auto& component)
			{
				DrawVec2Control("Offset", component.Offset, 0.01f, 0.0f);
				DrawVecControl("Radius", &component.Radius, 0.01f, 0.5f);

				float width = 118.0f;
				DrawVecControl("Density", &component.Density, 0.01f, 0.5f, width);
				DrawVecControl("Friction", &component.Friction, 0.01f, 0.0f, width);
				DrawVecControl("Restitution", &component.Restitution, 0.01f, 0.0f, width);
				DrawVecControl("Restitution Thrs", &component.Restitution, 0.01f, 0.5f, width);
			});

		DrawComponent<AudioListenerComponent>("AUDIO LISTENER", entity, [](auto& component)
			{
				ImGui::Checkbox("Enable", &component.Enable);
			});
	}

	template<typename T>
	bool SceneHierarchyPanel::DisplayAddComponentEntry(const std::string& entryName)
	{
		if (!m_SelectedEntity.HasComponent<T>())
		{
			if (ImGui::MenuItem(entryName.c_str()))
			{
				m_SelectedEntity.AddComponent<T>();
				ImGui::CloseCurrentPopup();
				return true;
			}
		}
		return false;
	}
}