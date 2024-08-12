// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace origin
{
	bool Math::DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
	{
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3], Pdum3{};

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}


		return true;
	}

	bool Math::DecomposeTransformEuler(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
	{
		// From glm::decompose in matrix_decompose.inl

		using namespace glm;
		using T = float;

		mat4 LocalMatrix(transform);

		// Normalize the matrix.
		if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
			return false;

		// First, isolate perspective.  This is the messiest.
		if (
			epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
			epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
		{
			// Clear the perspective partition
			LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
			LocalMatrix[3][3] = static_cast<T>(1);
		}

		// Next take care of translation (easy).
		translation = vec3(LocalMatrix[3]);
		LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

		vec3 Row[3];

		// Now get scale and shear.
		for (length_t i = 0; i < 3; ++i)
			for (length_t j = 0; j < 3; ++j)
				Row[i][j] = LocalMatrix[i][j];

		// Compute X scale factor and normalize first row.
		scale.x = length(Row[0]);
		Row[0] = detail::scale(Row[0], static_cast<T>(1));
		scale.y = length(Row[1]);
		Row[1] = detail::scale(Row[1], static_cast<T>(1));
		scale.z = length(Row[2]);
		Row[2] = detail::scale(Row[2], static_cast<T>(1));

		// At this point, the matrix (in rows[]) is orthonormal.
		// Check for a coordinate system flip.  If the determinant
		// is -1, then negate the matrix and the scaling factors.
#if 0
		Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
		if (dot(Row[0], Pdum3) < 0)
		{
			for (length_t i = 0; i < 3; i++)
			{
				scale[i] *= static_cast<T>(-1);
				Row[i] *= static_cast<T>(-1);
			}
		}
#endif

		rotation.y = asin(-Row[0][2]);
		if (cos(rotation.y) != 0) {
			rotation.x = atan2(Row[1][2], Row[2][2]);
			rotation.z = atan2(Row[0][1], Row[0][0]);
		}
		else {
			rotation.x = atan2(-Row[2][0], Row[1][1]);
			rotation.z = 0;
		}

		return true;
	}

	glm::vec3 Math::Normalize(const glm::vec3 &v)
	{
		float l = glm::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		if(l > 0.0f)
			return { v.x / l, v.y / l, v.z / l };
		return v;
	}

    glm::vec3 Math::WorldToScreen(const glm::vec3& worldPos, const glm::mat4 &modelTransform, const glm::mat4 &viewProjection, const glm::vec2 &screen)
    {
		glm::vec4 modelPos = modelTransform * glm::vec4(worldPos, 1.0f);
		glm::vec4 clipPos = viewProjection * modelPos;

		glm::vec3 ndcPos = glm::vec3(clipPos) / clipPos.w;

		glm::vec3 screenSpacePos;
		screenSpacePos.x = (ndcPos.x + 1.0f) * 0.5f * screen.x;
		screenSpacePos.y = (1.0f - ndcPos.y) * 0.5f * screen.y;
		screenSpacePos.z = ndcPos.z;
		return screenSpacePos;
    }

    glm::vec2 Math::GetNormalizedDeviceCoord(const glm::vec2 &mouse, const glm::vec2 &screen)
    {
        float x = (2.0f * mouse.x) / screen.x - 1;
        float y = 1.0f - (2.0f * mouse.y) / screen.y;
        return glm::vec2(x, y);
    }

    glm::vec4 Math::GetHomogeneouseClipCoord(const glm::vec2 &ndc)
    {
        return glm::vec4(ndc.x, ndc.y, -1.0f, 1.0f);
    }

    glm::vec4 Math::GetEyeCoord(glm::vec4 clipCoords, const glm::mat4 &projectionMatrix)
    {
        glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
        glm::vec4 eyeCoords = inverseProjection * clipCoords;
        return glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);
    }

    glm::vec3 Math::GetWorldCoord(const glm::vec4 &eyeCoords, const glm::mat4 &viewMatrix)
    {
        glm::vec4 worldCoords = glm::inverse(viewMatrix) * eyeCoords;
        return glm::normalize(glm::vec3(worldCoords));
    }

    glm::vec3 Math::GetRayOrthographic(const glm::vec2 & coord, const glm::vec2 & screen, const glm::mat4 & projection, const glm::mat4 & view, glm::vec3 *rayOrigin, const glm::vec3 & camForward)
    {
        glm::vec2 ndc = GetNormalizedDeviceCoord(coord, screen);
        glm::vec4 hmc = GetHomogeneouseClipCoord({ ndc.x, -ndc.y });
        glm::vec3 rayDirection = glm::vec3(0.0f);
        *rayOrigin = glm::vec3(0.0f);

        // calculate ray origin and direction for orthographic projection
        glm::mat4 invViewProj = glm::inverse(projection * view);

        // ray origin (on near plane)
        *rayOrigin = invViewProj * glm::vec4(ndc.x, -ndc.y, -1.0f, 1.0f);
        *rayOrigin /= 1.0f;
        rayDirection = -glm::normalize(camForward);

        return rayDirection;
    }

    glm::vec3 Math::GetRayPerspective(const glm::vec2 &coord, const glm::vec2 &screen, const glm::mat4 &projection, const glm::mat4 &view)
    {
        glm::vec2 ndc = GetNormalizedDeviceCoord(coord, screen);
        glm::vec4 hmc = GetHomogeneouseClipCoord({ ndc.x, -ndc.y });
        glm::vec4 eye = GetEyeCoord(hmc, projection);

        return GetWorldCoord(eye, view);;
    }

	glm::vec3 Math::GetMouseRayWorldSpace(const glm::vec2 &rayNDC, const glm::mat4 &viewProjection)
	{
		glm::mat4 invViewProjection = glm::inverse(viewProjection);
		glm::vec4 rayWorld = invViewProjection * glm::vec4(rayNDC.x, rayNDC.y, 1.0f , 1.0f);
		return glm::normalize(glm::vec3(rayWorld));
	}

	bool Math::RaySphereIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection, const glm::vec3 &sphereCenter, float sphereRadius)
	{
		glm::vec3 oc = rayOrigin - sphereCenter;
		float a = glm::dot(rayDirection, rayDirection);
		float b = 2.0f * glm::dot(oc, rayDirection);
		float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
		float discriminant = b * b - 4 * a * c;
		return (discriminant > 0);
	}

	bool Math::RayAABBIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection, const glm::vec3 &boxMin, const glm::vec3 &boxMax)
	{
		glm::vec3 invDir = 1.0f / rayDirection;
		glm::vec3 tMin = (boxMin - rayOrigin) * invDir;
		glm::vec3 tMax = (boxMax - rayOrigin) * invDir;
		glm::vec3 t1 = glm::min(tMin, tMax);
		glm::vec3 t2 = glm::max(tMin, tMax);

		float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
		float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);
		return tNear <= tFar && tFar > 0;
	}

}