// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "Grid.h"
#include "Origin\Renderer\Renderer2D.h"

namespace origin {

	void Grid::DrawVertical(EditorCamera& camera, glm::vec4 color)
	{
		float orthoSize = camera.GetOrthoSize();
		glm::vec2 cameraPosition = glm::vec2(camera.GetPosition());

		float lineSpacing = 1.0f;
		if (orthoSize >= 20.0f)
			lineSpacing = pow(10.0f, floor(log10(orthoSize)));

		float offset = -0.5f * lineSpacing;

		float minX = cameraPosition.x - orthoSize;
		float maxX = cameraPosition.x + orthoSize;
		float minY = cameraPosition.y - orthoSize;
		float maxY = cameraPosition.y + orthoSize;

		for (float x = floor(minX / lineSpacing) * lineSpacing; x <= maxX; x += lineSpacing)
			Renderer2D::DrawLine(glm::vec3(x + offset, minY, 1.0f), glm::vec3(x + offset, maxY, 1.0f), color);
		for (float y = floor(minY / lineSpacing) * lineSpacing; y <= maxY; y += lineSpacing)
			Renderer2D::DrawLine(glm::vec3(minX, y + offset, 1.0f), glm::vec3(maxX, y + offset, 1.0f), color);
	}


	void Grid::DrawHorizontal(glm::vec3 size, glm::vec4 color)
	{
	}

}