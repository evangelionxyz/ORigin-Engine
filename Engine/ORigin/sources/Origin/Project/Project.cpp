// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Project.h"
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Serializer/ProjectSerializer.h"
#include "Origin/Serializer/SceneSerializer.h"
#include "Origin/Serializer/MaterialSerializer.h"
#include "Origin/Profiler/Profiler.h"
#include "Origin/Utils/Utils.h"
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace origin {

	namespace Utils
	{
		std::string ReadFile(const std::string &filepath)
		{
			std::ifstream file(filepath);
			if (!file.is_open())
			{
				OGN_CORE_ASSERT(false, "[Project] Failed to open read file! {0}", filepath);
				return "";
			}
			std::stringstream stream;
			stream << file.rdbuf();
			file.close();
			return stream.str();
		}

		bool WriteFile(const std::string &filepath, const std::string &content)
		{
			std::ofstream file(filepath);
			if (!file.is_open())
			{
				OGN_CORE_ASSERT(false, "[Project] Failed to open and write file! {0}", filepath);
				return false;
			}

			file << content;
			file.close();
			return true;
		}

		void ReplaceAll(std::string &str, const std::string &from, const std::string &to)
		{
			size_t startPos = 0;
			while ((startPos = str.find(from, startPos)) != std::string::npos)
			{
				str.replace(startPos, from.length(), to);
				startPos += to.length();
			}
		}

		void GenerateFile(const std::string &templatePath, const std::string &outputPath, const std::string &projectName)
		{
			std::string content = ReadFile(templatePath);
			if (content.empty())
			{
				OGN_CORE_ERROR("[Project] Not content file!");
				return;
			}

			ReplaceAll(content, "{PROJECT_NAME}", projectName);
			if (!WriteFile(outputPath, content))
			{
				OGN_CORE_ASSERT(false, "[Project] Failed to generate file! {0}", outputPath);
			}
		}

		void CopyFile(const std::string &scrPath, const std::string &dstPath)
		{
			OGN_CORE_ASSERT(std::filesystem::exists(scrPath), "[Project Copy File] Failed to copy file! {0}", scrPath);
			std::filesystem::copy_file(scrPath, dstPath, std::filesystem::copy_options::overwrite_existing);
		}
	}

	void Project::SetStartScene(AssetHandle handle)
	{
		OGN_PROFILER_FUNCTION();

		if (GetEditorAssetManager()->IsAssetHandleValid(handle))
			m_Config.StartScene = handle;
	}

	std::filesystem::path Project::GetAssetAbsolutePath(const std::filesystem::path& path)
	{
		return GetAssetDirectory() / path;	
	}

	std::shared_ptr<Project> Project::New()
	{
		OGN_PROFILER_FUNCTION();

#ifdef _WIN32
		std::filesystem::path projectFilepath = FileDialogs::SaveFile("ORigin Project (*.oxproj)\0*.oxproj\0");
#elif __linux__
		std::filesystem::path projectFilepath = FileDialogs::SaveFile("ORigin Project | *.oxproj");
#endif
		if (projectFilepath == "")
			return nullptr;
		
		OGN_CORE_INFO("[Project New] {0}", projectFilepath.generic_string());
		std::string projectName = projectFilepath.stem().string();
		std::filesystem::path filepath = projectFilepath.parent_path();
		
		// Creating Project Folders
		std::filesystem::path assetDir = filepath / "Assets";
		if(!std::filesystem::exists(assetDir))
		{
			std::filesystem::create_directory(assetDir);
			std::filesystem::path scriptDir = assetDir / "Scripts";
			if(!std::filesystem::exists(scriptDir))
			{
				std::filesystem::create_directory(scriptDir);
			}
		}

		// Get premake5.lua template
		std::string premakeFilepath = std::filesystem::absolute("Resources/ScriptProjectGen/premake_template.lua").generic_string();
		std::string projectPremakeLocation = std::filesystem::absolute(filepath.generic_string() + "/premake5.lua").generic_string();
		Utils::GenerateFile(premakeFilepath, projectPremakeLocation, projectName);

#if defined(_WIN32)
		// Copying the build.bat
		std::string buildScriptPath = std::filesystem::absolute("Resources/ScriptProjectGen/build.bat").generic_string();
		std::string projectBuildScriptPath = filepath.generic_string() + "/build.bat";
		Utils::GenerateFile(buildScriptPath, projectBuildScriptPath, projectName);
		
#elif defined(OGN_PLATFORM_LINUX)
		// Copying the build.sh
		std::string buildScriptPath = std::filesystem::absolute("Resources/ScriptProjectGen/build.sh").generic_string();
		std::string projectBuildScriptPath = filepath.generic_string() + "/build.sh";
		Utils::CopyFile(buildScriptPath, projectBuildScriptPath);
#endif
		Utils::ExecuteScript(projectBuildScriptPath);

		std::shared_ptr<Project> newProject = std::make_shared<Project>();
		newProject->GetConfig().Name = projectName;
		// No start scene in new project
		newProject->GetConfig().StartScene = 0;
		newProject->GetConfig().ScriptModulePath = "bin/" + projectName + ".dll";

		ProjectSerializer serializer(newProject);
		if (serializer.Serialize(projectFilepath))
		{
			s_ActiveProject = newProject;
			s_ActiveProject->m_ProjectDirectory = filepath;

			std::shared_ptr<EditorAssetManager> editorAssetManager = std::make_shared<EditorAssetManager>();
			s_ActiveProject->m_AssetManager = editorAssetManager;

			// Create empty Asset Registry file
			editorAssetManager->SerializeAssetRegistry();

			return s_ActiveProject;
		}
		return nullptr;
	}

	std::shared_ptr<Project> Project::Open()
	{
		OGN_PROFILER_FUNCTION();
#ifdef _WIN32
		std::filesystem::path filepath = FileDialogs::OpenFile("ORigin Project (*.oxproj)\0*.oxproj\0");
#elif __linux__
															// we can use ; to separate extension
															// *.oxproj; *.txt ....
		std::filesystem::path filepath = FileDialogs::OpenFile("ORigin Project | *.oxproj");
#endif
		if (filepath.string().empty())
			return nullptr;

		return Load(filepath);
	}

	std::shared_ptr<Project> Project::Load(const std::filesystem::path& path)
	{
		OGN_PROFILER_FUNCTION();

		std::shared_ptr<Project> project = std::make_shared<Project>();

		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			project->m_ProjectDirectory = path.parent_path();
			s_ActiveProject = project;

			std::shared_ptr<EditorAssetManager> editorAssetManager = std::make_shared<EditorAssetManager>();
			editorAssetManager->DeserializeAssetRegistry();
			s_ActiveProject->m_AssetManager = editorAssetManager;
			
			return s_ActiveProject;
		}

		return nullptr;
	}

	bool Project::SaveActive()
	{
		OGN_PROFILER_FUNCTION();

		ProjectSerializer serializer(s_ActiveProject);
		return serializer.Serialize(s_ActiveProject->GetProjectPath());
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		OGN_PROFILER_FUNCTION();

		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Serialize(path))
		{
			s_ActiveProject->m_ProjectDirectory = path.parent_path();
			return true;
		}

		return false;
	}
}