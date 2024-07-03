#pragma once
#include "Origin/Project/Project.h"

namespace origin
{
	class ProjectSerializer
	{
	public:
		ProjectSerializer(std::shared_ptr<Project> project);

		bool Serialize(const std::filesystem::path& filepath);
		bool Deserialize(const std::filesystem::path& filepath);

	private:
		std::shared_ptr<Project> m_Project;
	};
}