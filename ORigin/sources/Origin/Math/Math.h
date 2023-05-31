// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include <glm\glm.hpp>

namespace origin
{
	class Math
	{
	public:
		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::vec3& outRotation, glm::vec3& outScale);
	};
}


