// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\VertexArray.h"
#include "Origin\Renderer\Buffer.h"
#include "Origin\Renderer\Shader.h"
#include "Origin\Renderer\Texture.h"

#include "Origin\Scene\Component\Component.h"
#include "Origin\Scene\Component\Camera.h"
#include "Origin\Scene\EditorCamera.h"

#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/RenderCommand.h"

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Origin
{
	class Renderer3D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);

		static void EndScene();
		static void Flush();

		static void DrawCube(const glm::mat4& transform, const std::shared_ptr<Texture2D>& texture, const glm::vec4& tintColor, int entityID = -1);
		static void DrawCube(const glm::mat4& transform, const glm::vec4& color, int entityID = -1);
		static void DrawCube(const glm::mat4& transform, SpriteRendererComponent& sprite, int entityID = -1);

		static void DrawLight(const glm::mat4& transform, glm::vec4 color, float intensity, int entityID = -1);

		static void DrawRect(const glm::vec3& position, const glm::vec2& size, glm::vec4& color = glm::vec4(1.0f), int entityID = -1);
		static void DrawRect(const glm::mat4& transform, glm::vec4& color = glm::vec4(1.0f), int entityID = -1);

	private:
		static void StartBatch();
		static void NextBatch();
	};

	struct LightingVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		float Intensity;

		int EntityID;
	};

	struct CubeVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;

		int EntityID;
	};

	struct Renderer3DData
	{
		static const uint32_t MaxTriangles = 1000;
		static const uint32_t MaxVertices = 4 * MaxTriangles;
		static const uint32_t MaxIndices = 6 * MaxTriangles;
		static const uint32_t MaxTextureSlots = 32;

		// Cube Data
		std::shared_ptr<Shader> CubeShader;
		std::shared_ptr<VertexArray> CubeVertexArray;
		std::shared_ptr<VertexBuffer> CubeVertexBuffer;

		uint32_t CubeIndexCount = 0;
		CubeVertex* CubeVertexBufferBase = nullptr;
		CubeVertex* CubeVertexBufferPtr = nullptr;
		glm::vec4 CubeVertexPosition[24];

		// Lighting Data
		std::shared_ptr<Shader> LightingShader;
		std::shared_ptr<VertexArray> LightingVertexArray;
		std::shared_ptr<VertexBuffer> LightingVertexBuffer;

		uint32_t LightingIndexCount = 0;
		LightingVertex* LightingVertexBufferBase = nullptr;
		LightingVertex* LightingVertexBufferPtr = nullptr;


		// Data
		std::shared_ptr<Texture2D> WhiteTexture;
		std::array<std::shared_ptr<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1;
	};

	static Renderer3DData s_3Ddata;
}

