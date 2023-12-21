#pragma once

#include "Origin\Scene\EditorCamera.h"
#include <glm\glm.hpp>

namespace origin {

	class Gizmos
	{
	public:
		static void DrawIcon(const EditorCamera& camera);
		static void DrawVerticalGrid(EditorCamera& camera, glm::vec4 color = glm::vec4(1.0f));
	};

}

