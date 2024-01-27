#pragma once

#include "Origin\Scene\EditorCamera.h"
#include <glm\glm.hpp>

namespace origin {

	class Gizmos
	{
	public:

		static void OnUpdate(const EditorCamera& camera);
		static void Draw2DVerticalGrid(const EditorCamera& camera);
		static void Draw2DOverlay(const EditorCamera& camera);
		static void DrawOverlay(const EditorCamera& camera);
		static void DrawIcons(const EditorCamera& camera);
	};

}

