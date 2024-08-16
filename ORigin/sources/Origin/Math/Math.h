// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Scene/Camera.h"

#include <assimp/postprocess.h>

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

#include "OBB.h"

namespace origin
{
	class Math
	{
	public:
		static bool DecomposeTransform(const glm::mat4& transform, glm::vec3& outTranslation, glm::quat& rotation, glm::vec3& scale);
		static bool DecomposeTransformEuler(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

		static glm::vec3 Normalize(const glm::vec3 &v);
		static glm::vec3 WorldToScreen(const glm::vec3 &worldPos, const glm::mat4 &modelTransform, const glm::mat4 &viewProjection, const glm::vec2 &screen);
        static glm::vec2 GetNormalizedDeviceCoord(const glm::vec2 &mouse, const glm::vec2 &screen);
		static glm::vec4 GetEyeCoord(glm::vec4 clipCoords, const glm::mat4 &projectionMatrix);
        static glm::vec3 GetWorldCoord(const glm::vec4 &eyeCoords, const glm::mat4 &viewMatrix);
        static glm::vec3 GetRayFromScreenCoords(const glm::vec2 &coord, const glm::vec2 &screen, const glm::mat4 &projection, const glm::mat4 &view, bool isPerspective, glm::vec3 &rayOrigin);

        static bool RaySphereIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection, const glm::vec3 &sphereCenter, float sphereRadius);

		static glm::mat4 AssimpToGlmMatrix(aiMatrix4x4 mat);
        static glm::vec3 AssimpToGlmVec3(aiVector3D vec);
        static glm::quat AssimpToGlmQuat(aiQuaternion quat);
	};
}


