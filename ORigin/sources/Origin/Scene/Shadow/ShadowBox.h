#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Origin\Scene\EditorCamera.h"

namespace origin
{
	class ShadowBox
	{
	private:
		static const float OFFSET;
		static const glm::vec4 UP;
		static const glm::vec4 FORWARD;
		static const float SHADOW_DISTANCE;

		float minX, maxX;
		float minY, maxY;
		float minZ, maxZ;

		glm::mat4 lightViewMatrix;

		float farHeight, farWidth, nearHeight, nearWidth;

		EditorCamera camera;
	public:
		ShadowBox(glm::mat4 lightViewMatrix, EditorCamera camera);
		void Update();

		glm::vec3 GetCenter();
		float GetWidth();
		float GetHeight();
		float GetLength();

	private:
		void CalculateFrustumVertices(const glm::mat4& rotation, const glm::vec3& forwardVector, const glm::vec3& centerNear, const glm::vec3& centerFar, glm::vec4* points);
		glm::vec4 CalculateLightSpaceFrustumCorner(const glm::vec3& startPoint, const glm::vec3& direction, float width);
		glm::mat4 CalculateCameraRotationMatrix();
		void CalculateWidthsAndHeights();

	};
}

