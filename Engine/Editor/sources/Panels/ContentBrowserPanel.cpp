// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "ContentBrowserPanel.h"
#include "../EditorLayer.h"

#include "Origin/Serializer/MaterialSerializer.h"
#include "Origin/Asset/AssetImporter.h"
#include "Origin/Asset/AssetManager.h"
#include "Origin/Project/Project.h"
#include "Origin/Utils/Utils.h"
#include "Origin/Utils/StringUtils.h"

#ifdef OGN_PLATFORM_WINDOWS
    #include <Windows.h>
    #include <shellapi.h>
#endif

namespace origin
{
    static u32 itemRenderCount = 0;

    static void OpenFile(const std::filesystem::path &filepath)
    {
#ifdef OGN_PLATFORM_WINDOWS
        ShellExecuteA(nullptr, "open", filepath.string().c_str(), nullptr, nullptr, SW_SHOWDEFAULT);
#endif
    }

    ContentBrowserPanel::ContentBrowserPanel(const std::shared_ptr<Project>& project)
        : m_Project(project), m_ThumbnailCache(std::make_shared<ThumbnailCache>(project)),
        m_BaseDirectory(m_Project->GetAssetDirectory()), m_CurrentDirectory(m_BaseDirectory)
    {
        m_TreeNodes.push_back(TreeNode(".", 0));
        m_IconMap["backward_button_icon"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic_backward_bt.png");
        m_IconMap["forward_button_icon"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic_forward_bt.png");
        m_IconMap["directory_icon"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_folder.png");
        m_IconMap["unknown"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic_file.png");
        m_IconMap[".cs"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_csharp_script.png");
        m_IconMap[".glsl"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_glsl.png");
        m_IconMap[".org"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_scene.png");
        m_IconMap[".mp3"] = TextureImporter::LoadTexture2D("Resources/UITextures/ic/ic_audio_source.png");
        
        RefreshAssetTree();
    }

    void ContentBrowserPanel::OnImGuiRender()
    {
        if (!m_Project)
            return;
        DrawContentBrowser();
    }

    void ContentBrowserPanel::ShowFileTree(const std::filesystem::path &directory)
    {
        for (const auto &entry : std::filesystem::directory_iterator(directory))
        {
            const std::filesystem::path &path = entry.path();
            std::string filename = path.filename().string();

            ImGuiTreeNodeFlags flags = (m_SelectedFileTree == entry.path() ? ImGuiTreeNodeFlags_Selected : 0) 
                | ImGuiTreeNodeFlags_OpenOnArrow
                | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_SpanFullWidth;

            if (!entry.is_directory())
                flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

            bool opened = ImGui::TreeNodeEx(filename.c_str(), flags, filename.c_str());

            if (ImGui::IsItemHovered(ImGuiMouseButton_Left))
            {
                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                    m_SelectedFileTree = entry.path();
                
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (std::filesystem::directory_entry(m_SelectedFileTree).is_directory())
                    {
                        m_BackwardPathStack.push(m_CurrentDirectory);
                        m_CurrentDirectory = m_SelectedFileTree;
                    }
                }
            }

            if (opened && entry.is_directory())
            {
                ShowFileTree(path);
                ImGui::TreePop();
            }
        }
    }

    void ContentBrowserPanel::DrawNavButton()
    {
        ImGuiWindowFlags childFlags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;
        ImGui::BeginChild("navigation_button", ImVec2(ImGui::GetContentRegionAvail().x, 30.0f), false, childFlags);

        // Navigation Button
        ImVec2 navigation_bt_size = ImVec2(23.0f, 23.0f);
        
        // Push Button Colors
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

        // Backward Button
        std::shared_ptr<Texture2D> navButtonTexture = m_IconMap.at("backward_button_icon");

        bool clicked = ImGui::ImageButton("backward_button", (void *)(uintptr_t)(navButtonTexture->GetID()), navigation_bt_size, ImVec2(0, 1), ImVec2(1, 0)) && !m_Project->GetActiveScene()->IsFocusing();
        if (clicked)
        {
            if (!m_BackwardPathStack.empty())
            {
                m_ForwardPathStack.push(m_CurrentDirectory);
                m_CurrentDirectory = m_BackwardPathStack.top();
                m_BackwardPathStack.pop();
            }
            clicked = false;
        }
        
        // Forward button
        ImGui::SameLine();
        navButtonTexture = m_IconMap.at("forward_button_icon");
        clicked = ImGui::ImageButton("forward_button", (void*)(uintptr_t)(navButtonTexture->GetID()), navigation_bt_size, ImVec2(0, 1), ImVec2(1, 0));

        if (clicked)
        {
            if (!m_ForwardPathStack.empty())
            {
                m_BackwardPathStack.push(m_CurrentDirectory);
                m_CurrentDirectory = m_ForwardPathStack.top();
                m_ForwardPathStack.pop();
            }

            clicked = false;
        }

        ImGui::SameLine();
        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::SliderInt("Thumbnail Size", &m_ThumbnailSize, 24, 256);

        ImGui::EndChild();
    }

    void ContentBrowserPanel::DrawContentBrowser()
    {
        ImGui::Begin("Content Browser");

        DrawNavButton();

        ImGui::BeginChild("left_item_browser", {300.0f, 0.0f}, ImGuiChildFlags_ResizeX);
        {
            ShowFileTree(Project::GetActiveAssetDirectory());
        }

        ImGui::EndChild();
        ImGui::SameLine();

        ImGui::BeginChild("item_browser", {0.0f, 0.0f}, false);
        static f32 padding = 10.0f;
        const f32 cellSize = m_ThumbnailSize + padding;
        const f32 panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = static_cast<int>(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, nullptr, false);

        TreeNode *node = m_TreeNodes.data();
        const auto &relativePath = std::filesystem::relative(m_CurrentDirectory, Project::GetActiveAssetDirectory());
        for (const auto &path : relativePath)
        {
            if (node->Path == relativePath)
                break;

            if (node->Children.find(path) != node->Children.end())
                node = &m_TreeNodes[node->Children[path]];
        }

        for (auto &[item, tree_node_index] : node->Children)
        {
            std::string filenameStr = item.generic_string();
            ImGui::PushID(filenameStr.c_str());
            const std::shared_ptr<Texture2D> thumbnail = DirectoryIcons(std::filesystem::directory_entry(m_CurrentDirectory / item));
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

            f32 thumbnailHeight = m_ThumbnailSize * ((f32)thumbnail->GetHeight() / (f32)thumbnail->GetWidth());
            f32 diff = (f32)(m_ThumbnailSize - thumbnailHeight);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + diff);

            ImGui::ImageButton(filenameStr.c_str(), (void *)(uintptr_t)(thumbnail->GetID()), { (f32)m_ThumbnailSize, (f32)thumbnailHeight }, { 0, 1 }, { 1, 0 });

            if (ImGui::BeginDragDropSource())
            {
                AssetHandle handle = m_TreeNodes[tree_node_index].Handle;
                ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", &handle, sizeof(AssetHandle));

                ImGui::BeginTooltip();
                ImGui::Text("%llu", (u64)handle);
                ImGui::EndTooltip();

                ImGui::EndDragDropSource();
            }

            ImGui::PopStyleColor();

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && !m_Project->GetActiveScene()->IsFocusing())
            {
                if (std::filesystem::directory_entry(m_CurrentDirectory / item).is_directory())
                {
                    m_BackwardPathStack.push(m_CurrentDirectory);
                    m_CurrentDirectory /= item.filename();
                }

                if (item.extension() == ".sprite")
                {
                    SpriteSheetEditorPanel::GetInstance()->SetSelectedSpriteSheet(m_TreeNodes[tree_node_index].Handle);
                    SpriteSheetEditorPanel::GetInstance()->Open();
                    ImGui::SetWindowFocus("Sprite Sheet Editor");
                }
                else if (item.extension() == ".mat")
                {
                    MaterialEditorPanel::GetInstance()->SetSelectedMaterial(m_TreeNodes[tree_node_index].Handle);
                    MaterialEditorPanel::GetInstance()->Open();
                    ImGui::SetWindowFocus("Material Editor");
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
                        SpriteSheetEditorPanel::GetInstance()->CreateNewSpriteSheet();
                        SpriteSheetEditorPanel::GetInstance()->SetMainTexture(m_TreeNodes[tree_node_index].Handle);
                        SpriteSheetEditorPanel::GetInstance()->Serialize(m_CurrentDirectory / (item.stem().string() + ".sprite"));
                        SpriteSheetEditorPanel::GetInstance()->SetSelectedSpriteSheet(m_TreeNodes[tree_node_index].Handle);
                        RefreshAssetTree();
                        SpriteSheetEditorPanel::GetInstance()->Open();
                        ImGui::SetWindowFocus("Sprite Sheet Editor");
                    }
                }

                if (item.extension() == ".org")
                {
                    if (ImGui::MenuItem("Set As Start Scene"))
                    {
                        AssetHandle handle = m_TreeNodes[tree_node_index].Handle;
                        Project::GetActive()->SetStartScene(handle);

                        RefreshAssetTree();
                    }
                }

                if (ImGui::MenuItem("Remove From Project"))
                {
                    AssetHandle handle = m_TreeNodes[tree_node_index].Handle;
                    Project::GetActive()->GetEditorAssetManager()->RemoveAsset(handle);

                    u32 index = tree_node_index;

                    node->Children.erase(item);
                    m_TreeNodes.erase(m_TreeNodes.begin() + index);
                    // reset the node
                    u32 count = 0;
                    for (auto n : m_TreeNodes)
                    {
                        if (node->Children.find(n.Path) != node->Children.end())
                            node->Children[n.Path] = count;
                        count++;
                    }

                    RefreshAssetTree();
                }

                if (ImGui::MenuItem("Rename"))
                {
                    m_Renaming = true;
                    m_RenamePath = item;
                    strncpy(m_RenameBuffer, filenameStr.c_str(), sizeof(m_RenameBuffer) - 1);
                }

                ImGui::EndPopup();
            }

            if (m_Renaming && !m_Renamed && m_RenamePath == item)
            {
                if (ImGui::InputText("##Rename", m_RenameBuffer, sizeof(m_RenameBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    std::filesystem::path newPath = m_CurrentDirectory / m_RenameBuffer;
                    if (!std::filesystem::exists(newPath))
                    {
                        std::filesystem::path renamingItem = item;
                        std::filesystem::rename(m_CurrentDirectory / renamingItem, newPath);

                        u32 index = tree_node_index;
                        node->Children.erase(renamingItem);
                        node->Children[m_RenameBuffer] = index;
                        m_TreeNodes[index].Path = newPath.filename();

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

                        m_Renamed = true;
                    }
                }
            }
            else
            {
                ImGui::TextWrapped("%s", filenameStr.c_str());
            }

            ImGui::NextColumn();
            ImGui::PopID();

            if (m_Renamed)
            {
                RefreshAssetTree();
                m_Renamed = false;
                break;
            }
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
                        RefreshAssetTree();
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

                if (ImGui::MenuItem("Open in Explorer", nullptr))
                {
                    OpenFile(m_CurrentDirectory);
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
        const std::filesystem::path asset_path = Project::GetActiveAssetDirectory();
        LoadAssetTree(asset_path);
    }

    void ContentBrowserPanel::LoadAssetTree(const std::filesystem::path &directory)
    {
        const std::filesystem::path asset_path = Project::GetActiveAssetDirectory();
        const AssetRegistry &asset_registry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();

        for (const auto &entry : std::filesystem::directory_iterator(directory))
        {
            const std::filesystem::path &current_relative_path = std::filesystem::relative(entry.path(), asset_path);

            u32 current_node_index = 0;
            for (auto path : current_relative_path)
            {
                auto it = m_TreeNodes[current_node_index].Children.find(path.generic_string());
                if (it != m_TreeNodes[current_node_index].Children.end())
                {
                    current_node_index = it->second;
                }
                else
                {
                    AssetHandle asset_handle_id = 0;
                    if (!std::filesystem::is_directory(path))
                    {
                        std::string relative_asset_path = std::filesystem::relative(entry.path(), asset_path).generic_string();
                        asset_handle_id = Project::GetActive()->GetEditorAssetManager()->ImportAsset(relative_asset_path);
                    }

                    TreeNode new_node(path, asset_handle_id);
                    new_node.Parent = current_node_index;

                    m_TreeNodes.push_back(new_node);
                    m_TreeNodes[current_node_index].Children[path] = static_cast<int>(m_TreeNodes.size()) - 1;
                    current_node_index = static_cast<int>(m_TreeNodes.size()) - 1;
                }
            }

            if (entry.is_directory())
            {
                // absolute path
                LoadAssetTree(entry.path());
            }
        }
    }

    std::shared_ptr<Texture2D> ContentBrowserPanel::DirectoryIcons(const std::filesystem::directory_entry &dirEntry)
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
