// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once

namespace origin {

	struct AudioConfig
	{
		std::string Name;
		std::filesystem::path Filepath;

		bool IsLooping;
	};

	class AudioSerializer
	{
	public:

	private:
		AudioConfig m_Spec;
	};
}

