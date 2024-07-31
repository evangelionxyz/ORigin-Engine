#pragma once
#include "MeshVertexData.h"
#include "Material.h"
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>

namespace origin
{
	class MeshRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void Begin(const Camera &camera, Shader *shader = nullptr);
        static void StartBatch();
        static void NextBatch();
		static void End();

		static void DrawCube(const glm::mat4 &transform, const glm::vec4 &color, int entityID = -1);

		static void DrawCube(const glm::mat4 &transform, 
			const std::shared_ptr<Texture2D> &albedo, 
			const std::shared_ptr<Texture2D> &specular, 
			const glm::vec2 &tilingFactor, const glm::vec4 &color, int entityID = -1);

		static void DrawCube(const glm::mat4 &transform, Material *material, int entityID = -1);
		static void DrawSphere(const glm::mat4 &transform, const glm::vec4 &color, int entityID = -1);
		static void DrawSphere(const glm::mat4 &transform, Material *material, int entityID = -1);

		static Shader *GetShader();
	};
}


