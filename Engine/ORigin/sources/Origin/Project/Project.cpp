// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Project.h"
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Serializer/ProjectSerializer.h"
#include "Origin/Serializer/SceneSerializer.h"
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
		OGN_CORE_ASSERT(false, "[Project] Failed to open read file! {}", filepath);
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
		OGN_CORE_ASSERT(false, "[Project] Failed to open and write file! {}", filepath);
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
		OGN_CORE_ASSERT(false, "[Project] Failed to generate file! {}", outputPath);
	}
}

void CopyFile(const std::string &source_path, const std::string &destination_path)
{
	OGN_CORE_ASSERT(std::filesystem::exists(source_path), "[Project Copy File] Failed to copy file! {}", source_path);
	std::filesystem::copy_file(source_path, destination_path, std::filesystem::copy_options::overwrite_existing);
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

Ref<Project> Project::New()
{
	OGN_PROFILER_FUNCTION();

#ifdef _WIN32
	std::filesystem::path projectFilepath = FileDialogs::SaveFile("Origin Project (*.oxproj)\0*.oxproj\0");
#elif __linux__
	std::filesystem::path projectFilepath = FileDialogs::SaveFile("Origin Project | *.oxproj");
#endif
	if (projectFilepath == "")
		return nullptr;
		
	OGN_CORE_INFO("[Project New] {}", projectFilepath.generic_string());
	std::string project_name = projectFilepath.stem().string();
	std::filesystem::path filepath = projectFilepath.parent_path();
		
	// Creating Project Folders
	std::filesystem::path asset_dir = filepath / "Assets";
	if(!exists(asset_dir))
	{
		create_directory(asset_dir);
		std::filesystem::path script_dir = asset_dir / "Scripts";
		if(!exists(script_dir))
		{
			create_directory(script_dir);
		}
	}

	// Get premake5.lua template
	std::string premake_filepath = std::filesystem::absolute("Resources/ScriptProjectGen/premake_template.lua").generic_string();
	std::string project_premake_location = std::filesystem::absolute(filepath.generic_string() + "/premake5.lua").generic_string();
	Utils::GenerateFile(premake_filepath, project_premake_location, project_name);

#if defined(_WIN32)
	// Copying the build.bat
	std::string build_script_path = std::filesystem::absolute("Resources/ScriptProjectGen/build.bat").generic_string();
	std::string project_build_script_path = filepath.generic_string() + "/build.bat";
	Utils::GenerateFile(build_script_path, project_build_script_path, project_name);
		
#elif defined(OGN_PLATFORM_LINUX)
	// Copying the build.sh
	std::string build_script_path = std::filesystem::absolute("Resources/ScriptProjectGen/build.sh").generic_string();
	std::string project_build_script_path = filepath.generic_string() + "/build.sh";
	Utils::CopyFile(build_script_path, project_build_script_path);
#endif
	Utils::ExecuteScript(project_build_script_path);

	Ref<Project> newProject = CreateRef<Project>();
	newProject->GetConfig().Name = project_name;
	// No start scene in new project
	newProject->GetConfig().StartScene = 0;
	newProject->GetConfig().ScriptModulePath = "bin/" + project_name + ".dll";

	ProjectSerializer serializer(newProject);
	if (!serializer.Serialize(projectFilepath))
		return nullptr;
	
	s_ActiveProject = newProject;
	s_ActiveProject->m_ProjectDirectory = filepath;

	Ref<EditorAssetManager> editor_asset_manager = CreateRef<EditorAssetManager>();
	s_ActiveProject->m_AssetManager = editor_asset_manager;

	// Create empty Asset Registry file
	editor_asset_manager->SerializeAssetRegistry();
	return s_ActiveProject;
}

Ref<Project> Project::Open()
{
	OGN_PROFILER_FUNCTION();
#ifdef _WIN32
	std::filesystem::path filepath = FileDialogs::OpenFile("Origin Project (*.oxproj)\0*.oxproj\0");
#elif __linux__
	// we can use ; to separate extension
	// *.oxproj; *.txt ....
	std::filesystem::path filepath = FileDialogs::OpenFile("Origin Project | *.oxproj");
#endif
	if (filepath.string().empty())
		return nullptr;

	return Load(filepath);
}

Ref<Project> Project::Load(const std::filesystem::path& path)
{
	const Ref<Project> project = CreateRef<Project>();

	if (ProjectSerializer serializer(project); !serializer.Deserialize(path))
		return nullptr;
	
	project->m_ProjectDirectory = path.parent_path();
	s_ActiveProject = project;
	Ref<EditorAssetManager> editorAssetManager = CreateRef<EditorAssetManager>();
	editorAssetManager->DeserializeAssetRegistry();
	s_ActiveProject->m_AssetManager = editorAssetManager;
	return s_ActiveProject;
}

bool Project::SaveActive()
{
	ProjectSerializer serializer(s_ActiveProject);
	return serializer.Serialize(s_ActiveProject->GetProjectPath());
}

bool Project::SaveActive(const std::filesystem::path& path)
{
	if (ProjectSerializer serializer(s_ActiveProject); serializer.Serialize(path))
		return false;
	s_ActiveProject->m_ProjectDirectory = path.parent_path();
	return true;
}

}