// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "ContentBrowserPanel.h"
#include "../EditorLayer.h"

#include "Origin/Serializer/MaterialSerializer.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Project/Project.h"
#include "Origin/Utils/Utils.h"
#include "Origin/Utils/StringUtils.h"

#include <imgui.h>

namespace origin
{
	static uint32_t itemRenderCount = 0;

	ContentBrowserPanel::ContentBrowserPanel(const std::shared_ptr<Project>& project)
		: m_Project(project), m_ThumbnailCache(std::make_shared<ThumbnailCache>(project)), m_BaseDirectory(m_Project->GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
	{
		m_TreeNodes.push_back(TreeNode(".", 0));
		m_IconMap["backward_button_icon"] = TextureImporter::LoadTexture2D("Resources/UITextures/backward_icon.png");
		m_IconMap["forward_button_icon"] = TextureImporter::LoadTexture2D("Resources/UITextures/forward_icon.png");
		m_IconMap["directory_icon"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_folder.png");
		m_IconMap["asset_mode_icon"] = TextureImporter::LoadTexture2D("Resources/UITextures/asset_mode_icon.png");
		m_IconMap["file_mode_icon"] = TextureImporter::LoadTexture2D("Resources/UITextures/file_mode_icon.png");
		m_IconMap["unknown"] = TextureImporter::LoadTexture2D("Resources/UITextures/file_icon.png");
		m_IconMap[".cs"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_csharp_script.png");
		m_IconMap[".glsl"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_glsl.png");
		m_IconMap[".org"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_scene.png");
		m_IconMap[".mp3"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_audio_source.png");
		
		RefreshAssetTree();
		m_Mode = Mode::Asset;
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		if (!m_Project)
			return;

		DrawContentBrowser();
	}

	void ContentBrowserPanel::DrawNavButton()
	{
		ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
		ImGui::BeginChild("navigation_button", ImVec2(ImGui::GetContentRegionAvail().x, 30.0f), false, childFlags);

		// Navigation Button
		ImVec2 navBtSize = ImVec2(23.0f, 23.0f);
		
		// Push Button Colors
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		// Backward Button
		std::shared_ptr<Texture2D> navButtonTexture = m_IconMap.at("backward_button_icon");
		if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(navButtonTexture->GetRendererID()), navBtSize, ImVec2(0, 1), ImVec2(1, 0))
			&& !m_Project->GetActiveScene()->IsFocusing())
		{
			if (m_CurrentDirectory != Project::GetActiveAssetDirectory())
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
		
		// Forward button
		ImGui::SameLine();
		navButtonTexture = m_IconMap.at("forward_button_icon");
		ImGui::ImageButton(reinterpret_cast<ImTextureID>(navButtonTexture->GetRendererID()), navBtSize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();
		ImGui::PopStyleColor(3);

		// Push Button Colors
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_Mode == Mode::Asset ? ImVec4(0.0f, 0.8f, 0.0f, 1.0f) : ImVec4(0.8f, 0.0f, 0.8f, 1.0f));
		const char* label = m_Mode == Mode::Asset ? "Asset" : "File";
		navButtonTexture = m_Mode == Mode::Asset ? m_IconMap.at("asset_mode_icon") : m_IconMap.at("file_mode_icon");
		if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(navButtonTexture->GetRendererID()), navBtSize, ImVec2(0, 1), ImVec2(1, 0))
			&& !m_Project->GetActiveScene()->IsFocusing())
		{
			m_Mode = m_Mode == Mode::Asset ? Mode::FileSystem : Mode::Asset;
			RefreshAssetTree();
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::SliderInt("Thumbnail Size", &m_ThumbnailSize, 24, 256);

		ImGui::EndChild();
	}

	void ContentBrowserPanel::DrawContentBrowser()
	{
		ImGui::Begin("Content Browser");

		DrawNavButton();

		const auto canvasPos = ImGui::GetCursorScreenPos();
		const auto canvasSize = ImGui::GetContentRegionAvail();

		ImGui::BeginChild("item_browser", canvasSize, false);

		static float padding = 10.0f;
		const float cellSize = m_ThumbnailSize + padding;
		const float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = static_cast<int>(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, nullptr, false);

		if (m_Mode == Mode::Asset)
		{
			TreeNode* node = m_TreeNodes.data();
			const auto &relativePath = std::filesystem::relative(m_CurrentDirectory, Project::GetActiveAssetDirectory());
			for (const auto &path : relativePath)
			{
				if (node->Path == relativePath)
				{
					break;
				}

				if (node->Children.find(path) != node->Children.end())
				{
					node = &m_TreeNodes[node->Children[path]];
				}
			}

			for (auto &[item, treeNodeIndex] : node->Children)
			{
				bool shouldBreak = false;

				std::string filenameStr = item.generic_string();
				ImGui::PushID(filenameStr.c_str());
				const std::shared_ptr<Texture2D> thumbnail = DirectoryIcons(std::filesystem::directory_entry(m_CurrentDirectory / item));
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

				float thumbnailHeight = m_ThumbnailSize * ((float)thumbnail->GetHeight() / (float)thumbnail->GetWidth());
				float diff = (float)(m_ThumbnailSize - thumbnailHeight);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + diff);

				ImGui::ImageButton(reinterpret_cast<ImTextureID>(thumbnail->GetRendererID()), { (float)m_ThumbnailSize, (float)thumbnailHeight }, { 0, 1 }, { 1, 0 });

				if (ImGui::BeginDragDropSource())
				{
					AssetHandle handle = m_TreeNodes[treeNodeIndex].Handle;
					ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &handle, sizeof(AssetHandle));

					ImGui::BeginTooltip();
					ImGui::Text("%llu", handle);
					ImGui::EndTooltip();

					ImGui::EndDragDropSource();
				}

				ImGui::PopStyleColor();

				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && !m_Project->GetActiveScene()->IsFocusing())
				{
					if (std::filesystem::directory_entry(m_CurrentDirectory / item).is_directory())
						m_CurrentDirectory /= item.filename();

					if (item.extension() == ".sprite")
					{
						EditorLayer::Get().m_SpriteSheetEditor->SetSelectedSpriteSheet(m_TreeNodes[treeNodeIndex].Handle);
					}
					else if (item.extension() == ".mat")
					{
						EditorLayer::Get().m_MaterialEditor.SetSelectedMaterial(m_TreeNodes[treeNodeIndex].Handle);
					}
				}

				if (ImGui::BeginPopupContextItem("CONTENT_BROWSER_ITEM_CONTEXT", ImGuiPopupFlags_MouseButtonRight) && !m_Project->GetActiveScene()->IsFocusing())
				{
					Utils::CenteredText(Utils::CapitalizeWholeText(filenameStr).c_str());
					ImGui::Separator();

					if (item.extension() == ".png" || item.extension() == ".jpg")
					{
						if (ImGui::MenuItem("Create Sprite Sheet"))
						{
							SpriteSheetEditor::Get()->CreateNewSpriteSheet();
							SpriteSheetEditor::Get()->SetMainTexture(m_TreeNodes[treeNodeIndex].Handle);
							SpriteSheetEditor::Get()->Serialize(m_CurrentDirectory / (item.stem().string() + ".sprite"));

							RefreshAssetTree();
						}
					}

					if (item.extension() == ".org")
					{
						if (ImGui::MenuItem("Set As Start Scene"))
						{
							AssetHandle handle = m_TreeNodes[treeNodeIndex].Handle;
							Project::GetActive()->SetStartScene(handle);

							RefreshAssetTree();
						}
					}

					if (ImGui::MenuItem("Remove From Project"))
					{
						AssetHandle handle = m_TreeNodes[treeNodeIndex].Handle;
						Project::GetActive()->GetEditorAssetManager()->RemoveAsset(handle);

						uint32_t index = treeNodeIndex;

						node->Children.erase(item);
						m_TreeNodes.erase(m_TreeNodes.begin() + index);
						// reset the node
						uint32_t count = 0;
						for (auto n : m_TreeNodes)
						{
							if (node->Children.find(n.Path) != node->Children.end())
								node->Children[n.Path] = count;
							count++;
						}
						shouldBreak = true;
						
						RefreshAssetTree();
					}

					if (ImGui::MenuItem("Rename"))
					{
						m_Renaming = true;
						m_RenamePath = item;
						strncpy_s(m_RenameBuffer, filenameStr.c_str(), sizeof(m_RenameBuffer));
					}

					ImGui::EndPopup();
				}

				if (m_Renaming && m_RenamePath == item)
				{
					if (ImGui::InputText("##Rename", m_RenameBuffer, sizeof(m_RenameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
					{
						std::filesystem::path newPath = m_CurrentDirectory / m_RenameBuffer;
						if (!std::filesystem::exists(newPath))
						{
							std::filesystem::path renamingItem = item;
							std::filesystem::rename(m_CurrentDirectory / renamingItem, newPath);

							uint32_t index = treeNodeIndex;
							node->Children.erase(renamingItem);
							node->Children[m_RenameBuffer] = index;
							m_TreeNodes[index].Path = newPath.filename();
							shouldBreak = true;
							
                            // Refresh the asset registry
                            auto &assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
                            std::filesystem::path oldRelativePath = std::filesystem::relative(m_CurrentDirectory / renamingItem, Project::GetActiveAssetDirectory());
                            std::filesystem::path newRelativePath = std::filesystem::relative(newPath, Project::GetActiveAssetDirectory());

                            for (auto &[handle, metadata] : assetRegistry)
                            {
								auto genStr = oldRelativePath.generic_string();
                                if (metadata.Filepath.generic_string().find(genStr) == 0)
                                {
									std::filesystem::path relativePath = std::filesystem::relative(metadata.Filepath, oldRelativePath);
									metadata.Filepath = newRelativePath;
									if (relativePath != ".")
										metadata.Filepath /= relativePath;

									if (metadata.Type == AssetType::Material)
									{
										std::shared_ptr<Material> mat = AssetManager::GetAsset<Material>(handle);
										mat->SetName(metadata.Filepath.stem().string());
									}
									else if (metadata.Type == AssetType::Scene)
									{
                                        std::shared_ptr<Scene> scene = AssetManager::GetAsset<Scene>(handle);
                                        scene->SetName(metadata.Filepath.stem().string());
									}
                                }
                            }

                            // Serialize the updated asset registry
                            Project::GetActive()->GetEditorAssetManager()->SerializeAssetRegistry();
						}
						m_Renaming = false;
					}
				}
				else
				{
					ImGui::TextWrapped(filenameStr.c_str());
				}

				ImGui::NextColumn();
				ImGui::PopID();

				if (shouldBreak)
					break;
			}
		}
		else if(m_Mode == Mode::FileSystem)
		{
			uint32_t count = 0;
			for (auto &directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				count++;
			}

			ImGuiListClipper clipper;
			clipper.Begin(glm::ceil((float)count / (float)columnCount));

			bool first = true;
			while (clipper.Step())
			{
				auto it = std::filesystem::directory_iterator(m_CurrentDirectory);
				if (!first)
				{
					for (int i = 0; i < clipper.DisplayStart; i++)
					{
						for (int c = 0; c < columnCount && it != std::filesystem::directory_iterator(); c++)
						{
							it++;
						}
					}
				}

				for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
				{
					int c;
					for (c = 0; c < columnCount && it != std::filesystem::directory_iterator(); c++, it++)
					{
						const auto &directoryEntry = *it;

						const auto &path = directoryEntry.path();
						const std::string filenameStr = path.filename().string();

						ImGui::PushID(filenameStr.c_str());

						const auto &relativePath = std::filesystem::relative(path, Project::GetActiveAssetDirectory());
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
						auto thumbnail = DirectoryIcons(directoryEntry);
						float thumbnailHeight = m_ThumbnailSize * ((float)thumbnail->GetHeight() / (float)thumbnail->GetWidth());
						float diff = (float)(m_ThumbnailSize - thumbnailHeight);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() + diff);

						ImGui::ImageButton(reinterpret_cast<ImTextureID>(thumbnail->GetRendererID()), { (float)m_ThumbnailSize, (float)thumbnailHeight }, { 0, 1 }, { 1, 0 });
						ImGui::PopStyleColor();

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							std::string sizeString = Utils::BytesToString(thumbnail->GetEstimatedSize());
							ImGui::Text("Mem : %s", sizeString.c_str());
							ImGui::EndTooltip();
						}

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && !m_Project->GetActiveScene()->IsFocusing())
						{
							if (directoryEntry.is_directory())
								m_CurrentDirectory /= path.filename();
						}

						if (ImGui::BeginPopupContextItem() && !m_Project->GetActiveScene()->IsFocusing())
						{
							Utils::CenteredText(Utils::CapitalizeWholeText(filenameStr).c_str());
							ImGui::Separator();

							bool assetImported = false;
							const auto &assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
							for (const auto &[handle, metadata] : assetRegistry)
							{
								if (relativePath.generic_string() == metadata.Filepath)
								{
									assetImported = true;
									break;
								}
								assetImported = false;
							}

							if (!assetImported && !std::filesystem::is_directory(path))
							{
								if (ImGui::MenuItem("Import To Project"))
								{
									Project::GetActive()->GetEditorAssetManager()->ImportAsset(relativePath);
									RefreshAssetTree();
								}
							}

							ImGui::EndPopup();
						}

						ImGui::TextWrapped(filenameStr.c_str());
						ImGui::NextColumn();

						ImGui::PopID();
					}

					if (first && c != columnCount)
					{
						for (int i = 0; i < columnCount - c; i++)
						{
							ImGui::NextColumn();
						}
					}
					first = false;
				}
			}

			clipper.End();
		}

		ImGui::Columns(1);

        if (ImGui::BeginPopupContextWindow("CONTENT_BROWSER_CONTEXT", 
			ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems) && !m_Project->GetActiveScene()->IsFocusing())
        {
            if (ImGui::BeginMenu("CREATE"))
            {
                if (ImGui::MenuItem("New Folder", nullptr))
                {
                    const std::filesystem::path folder = m_CurrentDirectory / "New Folder";
                    if (!std::filesystem::exists(folder))
                    {
                        std::filesystem::create_directory(folder);
                    }
                }

                if (ImGui::MenuItem("Material", nullptr))
                {
                    std::shared_ptr<Material> material = Material::Create("Material");
                    const std::filesystem::path materialPath = m_CurrentDirectory / "Material.mat";
                    if (!std::filesystem::exists(materialPath))
                    {
                        MaterialSerializer::Serialize(materialPath, material);
						std::filesystem::path relativePath = std::filesystem::relative(materialPath, Project::GetActiveAssetDirectory());
						Project::GetActive()->GetEditorAssetManager()->ImportAsset(relativePath);

						RefreshAssetTree();
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Thumbnail Size"))
            {
                if (ImGui::MenuItem("Small"))
                    m_ThumbnailSize = 70;
                if (ImGui::MenuItem("Medium"))
                    m_ThumbnailSize = 90;
                if (ImGui::MenuItem("Large"))
                    m_ThumbnailSize = 110;

                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }

		ImGui::EndChild();

		ImGui::End();

		m_ThumbnailCache->OnUpdate();
	}

	void ContentBrowserPanel::RefreshAssetTree()
	{
		const auto &assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
		for (const auto &[handle, metadata] : assetRegistry)
		{
			uint32_t currentNodeIndex = 0;
			for (auto p : metadata.Filepath)
			{
				auto it = m_TreeNodes[currentNodeIndex].Children.find(p.generic_string());
				if (it != m_TreeNodes[currentNodeIndex].Children.end())
				{
					currentNodeIndex = it->second;
				}
				else
				{
					TreeNode newNode(p, handle);
					newNode.Parent = currentNodeIndex;

					m_TreeNodes.push_back(newNode);
					m_TreeNodes[currentNodeIndex].Children[p] = m_TreeNodes.size() - 1;

					currentNodeIndex = m_TreeNodes.size() - 1;
				}
			}
		}
	}

	std::shared_ptr<Texture2D> ContentBrowserPanel::DirectoryIcons(const std::filesystem::directory_entry& dirEntry)
	{
		const std::string& fileExtension = dirEntry.path().extension().string();
		auto relativePath = std::filesystem::relative(dirEntry.path(), Project::GetActiveAssetDirectory());

		std::shared_ptr<Texture2D> texture = m_IconMap.at("directory_icon");

		if (!dirEntry.is_directory())
		{
			if (fileExtension == ".png" || fileExtension == ".jpg")
			{
				texture = m_ThumbnailCache->GetOrCreateThumbnail(relativePath);
				if (!texture)
					texture = m_IconMap.at("unknown");
			}
			else if (m_IconMap.find(fileExtension) == m_IconMap.end())
				texture = m_IconMap.at("unknown");
			else
				texture = m_IconMap.at(fileExtension);
		}

		return texture;
	}

}
