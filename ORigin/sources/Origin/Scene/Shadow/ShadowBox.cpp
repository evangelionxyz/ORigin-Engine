#include "pch.h"
#include "ShadowBox.h"

namespace origin
{
	const float ShadowBox::OFFSET = 10;
	const float ShadowBox::SHADOW_DISTANCE = 100;

	ShadowBox::ShadowBox(glm::mat4 lightViewMatrix, EditorCamera camera)
		: lightViewMatrix(lightViewMatrix), camera(camera)
	{
		CalculateWidthsAndHeights();
	}

	void ShadowBox::Update()
	{
		glm::vec3 toFar = camera.GetForwardDirection() * SHADOW_DISTANCE;
		glm::vec3 toNear = camera.GetForwardDirection() * camera.GetNear();
		glm::vec3 centerNear = toNear + camera.GetPosition();
		glm::vec3 centerFar = toFar + camera.GetPosition();

		glm::vec4 points[8];
		CalculateFrustumVertices(CalculateCameraRotationMatrix(), camera.GetForwardDirection(), centerNear, centerFar, points);

		bool first = true;
		for (int i = 0; i < 8; ++i)
		{
			const glm::vec4& point = points[i];

			if (first)
			{
				minX = point.x;
				maxX = point.x;
				minY = point.y;
				maxY = point.y;
				minZ = point.z;
				maxZ = point.z;
				first = false;
				continue;
			}

			if (point.x > maxX)
			{
				maxX = point.x;
			}
			else if (point.x < minX)
			{
				minX = point.x;
			}
			if (point.y > maxY)
			{
				maxY = point.y;
			}
			else if (point.y < minY)
			{
				minY = point.y;
			}
			if (point.z > maxZ)
			{
				maxZ = point.z;
			}
			else if (point.z < minZ)
			{
				minZ = point.z;
			}
		}

		maxZ += OFFSET;
	}

	glm::vec3 ShadowBox::GetCenter()
	{
		float x = (minX + maxX) / 2.0f;
		float y = (minY + maxY) / 2.0f;
		float z = (minZ + maxZ) / 2.0f;
		glm::vec4 cen = glm::vec4(x, y, z, 1.0f);
		glm::mat4 invertedLight = glm::inverse(lightViewMatrix);
		return glm::vec3(invertedLight * cen);
	}

	float ShadowBox::GetWidth()
	{
		return maxX - minX;
	}

	float ShadowBox::GetHeight()
	{
		return maxY - minY;
	}

	float ShadowBox::GetLength()
	{
		return maxZ - minZ;
	}


	void ShadowBox::CalculateFrustumVertices(const glm::mat4& rotation, const glm::vec3& forwardVector, const glm::vec3& centerNear, const glm::vec3& centerFar, glm::vec4* points)
	{
		glm::vec3 upVector = glm::vec3(rotation * glm::vec4(UP));
		glm::vec3 rightVector = glm::cross(forwardVector, upVector);
		glm::vec3 downVector = -upVector;
		glm::vec3 leftVector = -rightVector;
		glm::vec3 farTop = centerFar + (upVector * farHeight);
		glm::vec3 farBottom = centerFar + (downVector * farHeight);
		glm::vec3 nearTop = centerNear + (upVector * nearHeight);
		glm::vec3 nearBottom = centerNear + (downVector * nearHeight);

		points[0] = CalculateLightSpaceFrustumCorner(farTop, rightVector, farWidth);
		points[1] = CalculateLightSpaceFrustumCorner(farTop, leftVector, farWidth);
		points[2] = CalculateLightSpaceFrustumCorner(farBottom, rightVector, farWidth);
		points[3] = CalculateLightSpaceFrustumCorner(farBottom, leftVector, farWidth);
		points[4] = CalculateLightSpaceFrustumCorner(nearTop, rightVector, nearWidth);
		points[5] = CalculateLightSpaceFrustumCorner(nearTop, leftVector, nearWidth);
		points[6] = CalculateLightSpaceFrustumCorner(nearBottom, rightVector, nearWidth);
		points[7] = CalculateLightSpaceFrustumCorner(nearBottom, leftVector, nearWidth);
	}

	glm::vec4 ShadowBox::CalculateLightSpaceFrustumCorner(const glm::vec3& startPoint, const glm::vec3& direction, float width)
	{
		glm::vec3 point = startPoint + (direction * width);
		glm::vec4 point4f = glm::vec4(point.x, point.y, point.z, 1.0f);
		return lightViewMatrix * point4f;
	}

	glm::mat4 ShadowBox::CalculateCameraRotationMatrix()
	{
		glm::mat4 rotation(1.0f);
		rotation = glm::rotate(rotation, glm::radians(-camera.GetYaw()), glm::vec3(0, 1, 0));
		rotation = glm::rotate(rotation, glm::radians(-camera.GetPitch()), glm::vec3(1, 0, 0));
		return rotation;
	}

	void ShadowBox::CalculateWidthsAndHeights()
	{
		farWidth = static_cast<float>(SHADOW_DISTANCE * tan(glm::radians(camera.GetFOV())));
		nearWidth = static_cast<float>(camera.GetNear() * tan(glm::radians(camera.GetFOV())));
		farHeight = farWidth / camera.GetAspectRatio();
		nearHeight = nearWidth / camera.GetAspectRatio();
	}
}
