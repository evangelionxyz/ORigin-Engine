// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "RenderCommand.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Scene/Components/Components.h"
#include "Origin/Scene/EditorCamera.h"

namespace origin {

	struct RenderData
	{
		static const uint32_t MaxTriangles = 1000;
		static const uint32_t MaxVertices = MaxTriangles * 28;
		static const uint32_t MaxQuadIndices = MaxTriangles * 6;
		static const uint32_t MaxTextureSlots = 32;
	};

	struct Statistics
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;
		uint32_t CircleCount = 0;
		uint32_t LineCount = 0;

		uint32_t CubeCount = 0;
		uint32_t SphereCount = 0;

		uint32_t GetTotalQuadVertexCount() const { return QuadCount * 4; }
		uint32_t GetTotalQuadIndexCount() const { return QuadCount * 6; }
		uint32_t GetTotalCubeVertexCount() const { return CubeCount * 24; }
		uint32_t GetTotalCubeIndexCount() const { return CubeCount * 36; }
        uint32_t GetTotalSphereVertexCount() const { return SphereCount * 544; }
        uint32_t GetTotalSphereIndexCount() const { return SphereCount * 768; }

		void Reset() { memset(this, 0, sizeof(Statistics)); };
	};

	class Renderer
	{
	public:
		static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static bool Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void SetCurrentShader(const std::shared_ptr<Shader> &shader);

		const static std::shared_ptr<Shader>& GetCurrentShader() { return s_GlobalShader; }
		static std::shared_ptr<Shader> GetShader(const std::string &name);
		static const std::unordered_map<std::string, std::shared_ptr<Shader>> GetShaderLibrary();

		static std::shared_ptr<Material> GetMaterial(const std::string &name);

		static std::shared_ptr<Texture2D> WhiteTexture;
		static std::shared_ptr<Texture2D> BlackTexture;

		static Statistics &GetStatistics();
		static RenderData s_RenderData;

        static glm::vec2 GetNormalizedDeviceCoord(const glm::vec2 &mouse, const glm::vec2 &screen);
        static glm::vec4 GetHomogeneouseClipCoord(const glm::vec2 &ndc);
        static glm::vec4 GetEyeCoord(glm::vec4 clipCoords, const glm::mat4 &projectionMatrix);
		static glm::vec3 GetWorldCoord(const glm::vec4 &eyeCoords, const glm::mat4 &viewMatrix);
		static glm::vec3 GetRay(const glm::vec2 &mouse, const glm::vec2 &screen, const EditorCamera &camera, glm::vec3 *rayOrigin);
		static bool RayIntersectsSphere(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirection, const glm::vec3 &sphereCenter, float sphereRadius);

	private:
		static void LoadShaders();
		static void LoadMaterials();

		static std::shared_ptr<Shader> s_GlobalShader;
	};
}