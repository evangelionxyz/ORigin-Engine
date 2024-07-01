#include <pch.h>	
#include "Project.h"
#include "Origin\Utils\PlatformUtils.h"
#include "Origin\Serializer\ProjectSerializer.h"
#include <yaml-cpp\yaml.h>

namespace origin {

	static std::string premakeSolutionFile = R"(architecture "x64"
configurations {
    "Debug",
    "Release",
    "Dist"
}

flags {
    "MultiProcessorCompile"
}
)";

	static std::string premakeProjectFile = R"(
kind "SharedLib"
language "C#"
dotnetframework "4.8"
location "%{wks.location}"

targetdir ("%{prj.location}/Binaries")
objdir ("%{prj.location}/Intermediates")

files {
    "%{prj.location}/Assets/**.cs",
}

links {
    "ORigin-ScriptCore"
}

filter "system:windows"
systemversion "latest"

filter "configurations:Debug"
optimize "On"
symbols "Default"

filter "configurations:Release"
optimize "On"
symbols "Default"

filter "configurations:Dist"
optimize "Full"
symbols "Off"

engine_path = os.getenv("ORiginEngine")
ScriptCoreLib = path.join(engine_path, "Scripts/projects/ORigin-ScriptCore.lua")
group "ORiginEngine"
    include(ScriptCoreLib)
group ""
)";

	static std::string winGenerateFile = "call %ORiginEngine%\\Scripts\\premake\\premake5.exe vs2022";

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

		std::filesystem::path filepath = FileDialogs::SaveFile("ORigin Project (*.oxproj)\0*.oxproj\0");
		if (filepath.empty())
			return nullptr;

		std::string projectName = filepath.stem().string();

		const auto &assetDirectoryPath = filepath.parent_path() / "Assets";
		if(!std::filesystem::exists(assetDirectoryPath))
			std::filesystem::create_directory(assetDirectoryPath);

		std::fstream outfile;

		const auto &premakeFilepath = filepath.parent_path() / "premake5.lua";
		outfile = std::fstream(premakeFilepath, std::ios::out);
		if (outfile.is_open())
		{
			outfile << "workspace " << "\"" << projectName << "\"" << std::endl;
			outfile << premakeSolutionFile << std::endl;
			outfile << "project" << "\"" << projectName << "\"" << std::endl;
			outfile << premakeProjectFile;
			outfile.close();
		}

		const auto &winGenFilepath = filepath.parent_path() / "WinGen.bat";
		outfile = std::fstream(winGenFilepath, std::ios::out);
		if (outfile.is_open())
		{
			outfile << "cd /d %~dp0" << std::endl;
			outfile << winGenerateFile << std::endl;
			outfile << "call MSBuild " << projectName << ".sln";
			outfile.close();
		}

		// Check if the batch file exists and run
		if (std::filesystem::exists(winGenFilepath))
		{
			std::string absolutePath = std::filesystem::absolute(winGenFilepath).string();
			std::string expandedCmd = std::string("\"" + absolutePath + "\"");
			std::system(expandedCmd.c_str());
		}

		std::shared_ptr<Project> newProject = std::make_shared<Project>();

		newProject->GetConfig().Name = projectName;
		// No start scene in new project
		newProject->GetConfig().StartScene = 0;
		newProject->GetConfig().ScriptModulePath = "Binaries/" + projectName + ".dll";

		ProjectSerializer serializer(newProject);
		if (serializer.Serialize(filepath))
		{
			s_ActiveProject = newProject;
			s_ActiveProject->m_ProjectDirectory = filepath.parent_path();

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

		std::filesystem::path filepath = FileDialogs::OpenFile("ORigin Project (*.oxproj)\0*.oxproj\0");
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