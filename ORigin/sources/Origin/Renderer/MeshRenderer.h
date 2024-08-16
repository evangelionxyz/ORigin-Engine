#pragma once
#include "MeshVertexData.h"
#include "Material.h"
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace origin
{
	class MeshRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void Begin(const glm::mat4 &viewProjection);
		static void AttachShadow(uint32_t id = 0);
		static void Begin(const Camera &camera, Shader *shader = nullptr);
        static void StartBatch();
        static void NextBatch();
		static void End();

		static void DrawMesh(const glm::mat4 &viewProjection, const glm::mat4 &transform, const std::shared_ptr<VertexArray> &va, Shader *shader = nullptr);

		static void DrawCube(const glm::mat4 &transform, glm::vec4 color = glm::vec4(1.0f));

		static void DrawCube(const glm::mat4 &transform, 
			const std::shared_ptr<Texture2D> &albedo, 
			const std::shared_ptr<Texture2D> &specular, 
			const glm::vec2 &tilingFactor, const glm::vec4 &color);

		static void DrawCube(const glm::mat4 &transform, Material *material);
		static void DrawSphere(const glm::mat4 &transform, const glm::vec4 &color);
		static void DrawSphere(const glm::mat4 &transform, Material *material);

        static void DrawCapsule(const glm::mat4 &transform, const glm::vec4 &color);
        static void DrawCapsule(const glm::mat4 &transform, Material *material);

		static Shader *GetShader();
	};
}


