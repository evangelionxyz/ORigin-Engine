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
			lineSpacing = pow(5.0f, floor(log10(orthoSize)));

		float offset = -0.5f * lineSpacing;

		float minX = cameraPosition.x - orthoSize - camera.GetWidth() / 10.0f;
		float maxX = cameraPosition.x + orthoSize + camera.GetWidth() / 10.0f;
		float minY = cameraPosition.y - orthoSize - camera.GetHeight() / 10.0f;
		float maxY = cameraPosition.y + orthoSize + camera.GetHeight() / 10.0f;

		auto nx = floor(minX / lineSpacing) * lineSpacing;
		auto ny = floor(minY / lineSpacing) * lineSpacing;

		for (float x = nx; x <= maxX; x += lineSpacing)
			Renderer2D::DrawLine(glm::vec3(x + offset, minY, 1.0f), glm::vec3(x + offset, maxY, 1.0f), color);

		for (float y = ny; y <= maxY; y += lineSpacing)
			Renderer2D::DrawLine(glm::vec3(minX, y + offset, 1.0f), glm::vec3(maxX, y + offset, 1.0f), color);
	}


	void Grid::DrawHorizontal(glm::vec3 size, glm::vec4 color)
	{
	}

}