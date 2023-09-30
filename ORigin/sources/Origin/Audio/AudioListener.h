// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once

namespace origin {

	class AudioListener
	{
	public:
		AudioListener() = default;
		void Set(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up);
	};
}


