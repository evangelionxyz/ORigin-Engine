// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef RENDERER_H
#define RENDERER_H

#include "RenderCommand.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Scene/Components/Components.h"
#include "Origin/Renderer/Lighting/LightingManager.hpp"
#include "Origin/Scene/Camera/EditorCamera.h"

namespace origin {

#define CAMERA_BINDING            (0)
#define LIGHTING_BINDING          (1)
#define DIRECTIONAL_LIGHT_BINDING (2)
#define SPOT_LIGHT_BINDING        (3)
#define POINT_LIGHT_BINDING       (4)
#define AREA_LIGHT_BINDING        (5)
#define SHADOW_BINDING            (6)

#define MATERIAL_BINDING          (7)

struct RenderData
{
	static constexpr u32 max_triangles = 1024;
	static constexpr u32 max_vertices = max_triangles * 24;
	static constexpr u32 max_quad_indices = max_triangles * 6;
	static constexpr u32 max_texture_slots = 32;
};

struct Statistics
{
	u32 draw_calls = 0;
	u32 quad_count = 0;
	u32 circle_count = 0;
	u32 line_count = 0;
	u32 cube_count = 0;
	u32 sphere_count = 0;

	[[nodiscard]] u32 GetTotalQuadVertexCount() const { return quad_count * 4; }
	[[nodiscard]] u32 GetTotalQuadIndexCount() const { return quad_count * 6; }
	[[nodiscard]] u32 GetTotalCubeVertexCount() const { return cube_count * 24; }
	[[nodiscard]] u32 GetTotalCubeIndexCount() const { return cube_count * 36; }
	[[nodiscard]] u32 GetTotalSphereVertexCount() const { return sphere_count * 544; }
	[[nodiscard]] u32 GetTotalSphereIndexCount() const { return sphere_count * 768; }

	void Reset() { memset(this, 0, sizeof(Statistics)); };
};

class ShaderLibrary;
class Renderer
{
public:
	static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	static bool Init();
	static void Shutdown();

	static void OnWindowResize(u32 width, u32 height);
	static void SetCurrentShader(const Ref<Shader> &shader);


	const static Ref<Shader>& GetCurrentShader() { return s_GlobalShader; }
	static Ref<Shader> GetShader(const std::string &name);
	static ShaderLibrary &GetShaderLibrary();

	static Ref<Material> GetMaterial(const std::string &name);

	static Ref<Texture2D> WhiteTexture;
	static Ref<Texture2D> BlackTexture;

	static Statistics &GetStatistics();
	
	static LightingManager *lighting_manager;
	static RenderData render_data;

private:
	static void LoadShaders();
	static void LoadMaterials();

	static Ref<Shader> s_GlobalShader;
};
}

#endif