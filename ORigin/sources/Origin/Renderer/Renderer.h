// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "RenderCommand.h"
#include "UniformBuffer.h"

#include "Origin\Scene\EditorCamera.h"
#include "Origin\Scene\SceneCamera.h"

#include "Origin\Renderer\Texture.h"

#include "Origin\Scene\Components.h"

namespace origin {

	struct RenderData
	{
		static const uint32_t MaxTriangles = 20000;
		static const uint32_t MaxVertices = MaxTriangles * 28;
		static const uint32_t MaxQuadIndices = MaxTriangles * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps
	};

	struct Statistics
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;
		uint32_t CircleCount = 0;
		uint32_t LineCount = 0;
		uint32_t CubeCount = 0;

		uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
		uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
		void Reset() { memset(this, 0, sizeof(Statistics)); };
	};

	class Renderer
	{
	public:
		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

		static bool Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);
		static void OnUpdate();

		static std::shared_ptr<Shader> GetGShader(const std::string& name);
		static const std::unordered_map<std::string, std::shared_ptr<Shader>> GetSaderLibrary();

		static std::shared_ptr<Texture2D> WhiteTexture;

		static Statistics &GetStatistics();
		static RenderData s_RenderData;

	private:
		static void LoadShader();
	};
}