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
		static void Begin();
		static void Flush();
		static void End();

		static void Draw(const glm::mat4 &transform, std::span<MeshVertexData> vertices, std::span<uint32_t> indices);
	};
}


