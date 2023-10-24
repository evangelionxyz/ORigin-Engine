// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/glm.hpp>

namespace origin
{
	class Math
	{
	public:
		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::quat& rotation, glm::vec3& scale);
		static bool DecomposeTransformEuler(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);
	};
}


