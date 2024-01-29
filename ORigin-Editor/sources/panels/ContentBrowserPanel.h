// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\Texture.h"
#include "ThumbnailCache.h"

#include <string>
#include <unordered_map>
#include <filesystem>

namespace origin
{
	class ContentBrowserPanel
	{
	public:
		ContentBrowserPanel() = default;
		ContentBrowserPanel(const std::shared_ptr<Project>& project);
		void OnImGuiRender();
		
	private:
		void DrawNavButton();	
		void DrawContentBrowser();
		void RefreshAssetTree();
		std::shared_ptr<Texture2D> DirectoryIcons(const std::filesystem::directory_entry& dirExtension);
	
		std::shared_ptr<Project> m_Project;
		std::shared_ptr<ThumbnailCache> m_ThumbnailCache;

		std::filesystem::path m_BaseDirectory;
		std::filesystem::path m_CurrentDirectory;
		
		std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_IconMap;

		struct TreeNode
		{
			std::filesystem::path Path;
			AssetHandle Handle = 0;
			uint32_t Parent = static_cast<uint32_t>(-1);
			std::map<std::filesystem::path, uint32_t> Children;
			
			TreeNode(std::filesystem::path path, AssetHandle handle)
				: Path(std::move(path)), Handle(handle) {}
		};

		std::vector<TreeNode> m_TreeNodes;
		std::map<std::filesystem::path, std::vector<std::filesystem::path>> m_AssetTree;
		
		std::map<int, std::filesystem::path> m_HistoryList;
		int m_ForwardCount = 0;
		int m_ThumbnailSize = 90;

		enum class Mode
		{
			Asset = 0, FileSystem = 1
		};

		Mode m_Mode = Mode::Asset;
	};
}
