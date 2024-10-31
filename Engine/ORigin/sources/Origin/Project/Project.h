// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef PROJECT_H
#define PROJECT_H

#include "Origin/Core/Assert.h"
#include "Origin/Asset/Asset.h"
#include "Origin/Asset/EditorAssetManager.h"

#include <string>
#include <filesystem>

namespace origin
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";
		AssetHandle StartScene = 0;
		std::filesystem::path AssetDirectory = "Assets";
		std::filesystem::path AssetRegistry = "AssetRegistry";
		std::filesystem::path ScriptModulePath;
	};

	class Scene;
	class Project
	{
	public:
		const std::filesystem::path& GetProjectDirectory() { return m_ProjectDirectory; }
		const std::filesystem::path GetProjectPath() { return GetProjectDirectory() / (m_Config.Name + ".oxproj"); }
		std::filesystem::path GetAssetDirectory() { return GetProjectDirectory() / m_Config.AssetDirectory; }
		std::filesystem::path GetAssetRegistryPath() { return GetProjectDirectory() / m_Config.AssetRegistry; }
		std::filesystem::path GetAssetFileSystemPath(const std::filesystem::path& path) { return GetAssetDirectory() / path; }
		std::filesystem::path GetAssetAbsolutePath(const std::filesystem::path& path);
		

		static const std::filesystem::path GetActiveProjectPath()
		{
			OGN_CORE_ASSERT(s_ActiveProject, "Invalid Active Project");
			return s_ActiveProject->GetProjectPath();
		}

		static const std::filesystem::path& GetActiveProjectDirectory()
		{
			OGN_CORE_ASSERT(s_ActiveProject, "Invalid Active Project");
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetActiveAssetDirectory()
		{
			OGN_CORE_ASSERT(s_ActiveProject, "Invalid Active Project");
			return s_ActiveProject->GetAssetDirectory();
		}

		static std::filesystem::path GetActiveAssetFileSystemPath(const std::filesystem::path& path)
		{
			OGN_CORE_ASSERT(s_ActiveProject, "Invalid Active Project");
			return s_ActiveProject->GetAssetFileSystemPath(path);
		}

		static std::filesystem::path GetActiveAssetRegistryPath()
		{
			OGN_CORE_ASSERT(s_ActiveProject, "Invalid Active Project");
			return s_ActiveProject->GetAssetRegistryPath();
		}

		static Ref<Scene> GetActiveScene() 
		{
			return s_ActiveProject->m_ActiveScene;
		}

		static std::shared_ptr<Project> GetActive() 
		{ 
			return s_ActiveProject;
		}

		ProjectConfig& GetConfig() { return m_Config; }
		std::shared_ptr<AssetManagerBase> GetAssetManager() { return m_AssetManager; }
		std::shared_ptr<EditorAssetManager> GetEditorAssetManager() { return std::static_pointer_cast<EditorAssetManager>(m_AssetManager); }

		static std::shared_ptr<Project> New();
		static std::shared_ptr<Project> Open();
		static std::shared_ptr<Project> Load(const std::filesystem::path& path);

		static bool SaveActive();
		static bool SaveActive(const std::filesystem::path& path);
		static void SetActiveScene(const std::shared_ptr<Scene> &scene)
		{
			s_ActiveProject->m_ActiveScene = scene;
		}

		void SetStartScene(AssetHandle handle);

	private:
		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		Ref<AssetManagerBase> m_AssetManager;
		Ref<Scene> m_ActiveScene;
		inline static Ref<Project> s_ActiveProject;
	};

}

#endif