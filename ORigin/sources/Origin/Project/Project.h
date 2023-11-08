// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine
#pragma once

#include "Origin\Core\Assert.h"

#include "Origin\Asset\Asset.h"
#include "Origin\Asset\EditorAssetManager.h"
#include "Origin\Asset\RuntimeAssetManager.h"

#include <string>
#include <filesystem>

namespace origin
{
	struct ProjectConfig
	{
		AssetHandle StartScene;
		std::string Name = "Untitled";
		std::filesystem::path AssetDirectory = "Assets/";
		std::filesystem::path AssetRegistry = "AssetRegistry.oxr";
		std::filesystem::path ScriptModulePath;
	};

	class Project
	{
	public:
		const std::filesystem::path& GetProjectDirectory() { return m_ProjectDirectory; }
		std::filesystem::path GetAssetDirectory() { return GetProjectDirectory() / m_Config.AssetDirectory; }
		std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) { return GetAssetDirectory() / path; }
		std::filesystem::path GetAssetRegistryPath() { return GetAssetDirectory() / m_Config.AssetRegistry; }
		std::filesystem::path GetAssetAbsolutePath(const std::filesystem::path& path);

		static const std::filesystem::path& GetActiveProjectDirectory()
		{
			OGN_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetActiveAssetDirectory()
		{
			OGN_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetAssetDirectory();
		}

		static std::filesystem::path GetActiveAssetFileSystemPath(const std::filesystem::path& path)
		{
			OGN_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetAssetFileSystemPath(path);
		}

		static std::filesystem::path GetActiveAssetRegistryPath() 
		{ 
			OGN_CORE_ASSERT(s_ActiveProject);
			return s_ActiveProject->GetAssetRegistryPath();
		}
		
		static std::shared_ptr<Project> GetActive() { return s_ActiveProject; }

		ProjectConfig& GetConfig() { return m_Config; }
		std::shared_ptr<AssetManagerBase> GetAssetManager() { return m_AssetManager; }
		std::shared_ptr<EditorAssetManager> GetEditorAssetManager() { return std::static_pointer_cast<EditorAssetManager>(m_AssetManager); }
		std::shared_ptr<RuntimeAssetManager> GetRuntimeAssetManager() { return std::static_pointer_cast<RuntimeAssetManager>(m_AssetManager); }

		static std::shared_ptr<Project> New();
		static std::shared_ptr<Project> Open();
		static std::shared_ptr<Project> Load(const std::filesystem::path& path);

		static bool SaveActive(const std::filesystem::path& path);

	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		std::shared_ptr<AssetManagerBase> m_AssetManager;
		inline static std::shared_ptr<Project> s_ActiveProject;
	};

}
