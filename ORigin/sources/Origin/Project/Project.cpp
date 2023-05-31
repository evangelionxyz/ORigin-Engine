#include <pch.h>	
#include "Project.h"
#include "Origin\Utils\PlatformUtils.h"
#include "ProjectSerializer.h"

namespace origin
{
	std::shared_ptr<Project> Project::New()
	{
		s_ActiveProject = std::make_shared<Project>();
		return s_ActiveProject;
	}

	std::shared_ptr<Project> Project::Open()
	{
		std::filesystem::path filepath = FileDialogs::OpenFile("ORigin Project (*.oxproj)\0*.oxproj\0");
		if (filepath.string().empty())
			return nullptr;

		return Load(filepath);
	}

	std::shared_ptr<Project> Project::Load(const std::filesystem::path& path)
	{
		std::shared_ptr<Project> project = std::make_shared<Project>();

		ProjectSerializer serializer(project);
		if (serializer.Deserialize(path))
		{
			project->m_ProjectDirectory = path.parent_path();
			s_ActiveProject = project;
			return s_ActiveProject;
		}

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path& path)
	{
		ProjectSerializer serializer(s_ActiveProject);
		if (serializer.Serialize(path))
		{
			s_ActiveProject->m_ProjectDirectory = path.parent_path();
			return true;
		}

		return false;
	}

}