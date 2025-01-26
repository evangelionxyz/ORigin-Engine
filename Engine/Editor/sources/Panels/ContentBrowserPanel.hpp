// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef CONTENT_BROWSER_PANEL_HPP
#define CONTENT_BROWSER_PANEL_HPP

#include "Origin/Renderer/Texture.h"
#include "ThumbnailCache.hpp"

#include <string>
#include <unordered_map>
#include <filesystem>
#include <stack>

namespace origin {
class ContentBrowserPanel
{
public:
    ContentBrowserPanel() = default;
    ContentBrowserPanel(const Ref<Project>& project);
    void OnImGuiRender();
        
private:
    void ShowFileTree(const std::filesystem::path &directory);

    void DrawNavButton();	
    void DrawContentBrowser();

    void RefreshAssetTree();
    void RefreshEntryPathList();
    void LoadAssetTree(const std::filesystem::path &directory);

    Ref<Texture2D> DirectoryIcons(const std::filesystem::directory_entry& dirExtension);
    
    Ref<Project> m_Project;
    Ref<ThumbnailCache> m_ThumbnailCache;

    std::filesystem::path m_BaseDirectory;
    std::filesystem::path m_CurrentDirectory;
    std::filesystem::path m_SelectedFileTree;
        
    std::unordered_map<std::string, Ref<Texture2D>> m_IconMap;

    struct TreeNode
    {
        std::filesystem::path Path;
        std::map<std::filesystem::path, uint32_t> Children;
        AssetHandle Handle = 0;
        uint32_t Parent = static_cast<uint32_t>(-1);
        TreeNode(std::filesystem::path path, AssetHandle handle)
            : Path(std::move(path)), Handle(handle) {}
    };

    std::vector<TreeNode> m_TreeNodes;
    std::filesystem::path m_RenamePath;

    bool m_Renaming = false;
    bool m_Renamed = false;
    char m_RenameBuffer[256] = { 0 };
    std::map<std::filesystem::path, std::vector<std::filesystem::path>> m_AssetTree;
    int m_ThumbnailSize = 64;

    std::stack<std::filesystem::path> m_BackwardPathStack;
    std::stack<std::filesystem::path> m_ForwardPathStack;
    std::vector<std::filesystem::path> m_PathEntryList;
};

}

#endif