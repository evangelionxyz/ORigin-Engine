// Copyright (c) Evangelion Manuhutu | ORigin Engine

#ifndef RENDERER_H
#define RENDERER_H

#include "RenderCommand.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Scene/Components/Components.h"
#include "Origin/Renderer/Lighting/LightingManager.hpp"
#include "Origin/Scene/Camera/EditorCamera.h"
#include "Origin/Renderer/Materials/MaterialManager.hpp"

namespace origin {

#define UNIFORM_CAMERA_BINDING            (0)
#define UNIFORM_LIGHTING_BINDING          (1)
#define UNIFORM_DIRECTIONAL_LIGHT_BINDING (2)
#define UNIFORM_REFLECTION_BINDING        (3)

#define SPOT_LIGHT_BINDING        (3)
#define POINT_LIGHT_BINDING       (4)
#define AREA_LIGHT_BINDING        (5)

#define SKYBOX_BINDING            (1)

#define MATERIAL_BINDING          (6)
#define DIFFUSE_TEXTURE_BINDING   (0)
#define SPECULAR_TEXTURE_BINDING  (1)
#define ROUGHNESS_TEXTURE_BINDING (2)

#define STORAGE_BUFFER_MATERIAL_BINDING   (0)
#define STORAGE_BUFFER_SPOTLIGHT_BINDING  (1)
#define STORAGE_BUFFER_POINTLIGHT_BINDING (2)

struct RenderData
{
	static constexpr u32 max_triangles = 1024;
	static constexpr u32 max_vertices = max_triangles * 24;
	static constexpr u32 max_quad_indices = max_triangles * 6;
	static constexpr u32 max_texture_slots = 32;
};

struct CameraBufferData
{
    glm::mat4 view_projection;
    glm::vec3 position;
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
	
	static Ref<Shader> GetShader(const std::string &name);
	static ShaderLibrary &GetShaderLibrary();
	static Ref<Material> GetMaterial(const std::string &name);
	static Statistics &GetStatistics();
	const static Ref<Shader>& GetCurrentShader() { return global_shader; }
	
	static Ref<Texture2D> white_texture;
	static Ref<Texture2D> black_texture;
	static Ref<UniformBuffer> camera_uniform_buffer;
	static Ref<UniformBuffer> skybox_uniform_buffer;
	static RenderData render_data;

private:
	static void LoadShaders();
	static Ref<Shader> global_shader;
};

}

#endif