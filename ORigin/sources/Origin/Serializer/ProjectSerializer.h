// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef PROJECT_SERIALIZER_H
#define PROJECT_SERIALIZER_H

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

#endif