// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include <glm/glm.hpp>

namespace origin
{
	class AudioListener
	{
	public:
		AudioListener() = default;
		void Set(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up);
		void SetEnable(bool enable);
	};
}


