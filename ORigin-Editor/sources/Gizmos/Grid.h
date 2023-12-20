// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "Origin\Scene\EditorCamera.h"
#include <glm\glm.hpp>

namespace origin {

	class Grid
	{
	public:
		static void DrawVertical(EditorCamera& camera, glm::vec4 color = glm::vec4(1.0f));
		static void DrawHorizontal(glm::vec3 size, glm::vec4 color = glm::vec4(1.0f));
	};

}

