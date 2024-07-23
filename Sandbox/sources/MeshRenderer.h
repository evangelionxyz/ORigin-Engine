#pragma once
#include "MeshVertexData.h"

#include "Origin/Renderer/Texture.h"
#include <glm/gtc/type_ptr.hpp>
#include <span>

namespace origin
{
	class MeshRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void StartBatch();
		static void NextBatch();
		static void Begin(const Camera &camera);
		static void Flush();
		static void End();

		static void DrawCube(const glm::mat4 &transform, const glm::vec4 &color, int entityID = -1);
		static void DrawSphere(const glm::mat4 &transform, const glm::vec4 &color, int entityID = -1);
	};
}


