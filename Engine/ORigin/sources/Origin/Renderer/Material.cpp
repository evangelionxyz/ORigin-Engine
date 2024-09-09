// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "Material.h"
#include "Origin/Asset/AssetManager.h"

#include "Renderer.h"

namespace origin {

#define ALBEDO_MAP		"u_AlbedoMap"
#define SPECULAR_MAP	"u_SpecularMap"

	Material::Material(const std::string &name)
		: m_Name(name)
	{
		OGN_PROFILER_RENDERING();

		m_Shader = Renderer::GetShader("BatchMesh");
		m_Shader->Enable();
		m_UniformBuffer = UniformBuffer::Create(sizeof(MaterialBufferData), MATERIAL_BINDING);

		Albedo.Texture = Renderer::WhiteTexture;
		Metallic.Texture = Renderer::WhiteTexture;
	}

	Material::Material(const std::shared_ptr<Shader> &shader)
		: m_Shader(shader)
	{
		OGN_PROFILER_RENDERING();

		OGN_CORE_ASSERT(m_Shader, "[Material] Shader is uninitialized or empty!");
		m_Shader->Enable();

		m_UniformBuffer = UniformBuffer::Create(sizeof(MaterialBufferData), MATERIAL_BINDING);

        Albedo.Texture = Renderer::WhiteTexture;
        Metallic.Texture = Renderer::WhiteTexture;
	}

	void Material::Bind()
	{
		OGN_PROFILER_RENDERING();

		m_UniformBuffer->Bind();
		m_UniformBuffer->SetData(&BufferData, sizeof(MaterialBufferData));
		m_Shader->Enable();
	}

	void Material::Unbind()
	{
		OGN_PROFILER_RENDERING();

		m_Shader->Disable();
		m_UniformBuffer->Unbind();
	}

	void Material::AddShader(const std::shared_ptr<Shader> &shader)
	{
		m_Shader = shader;
	}

	bool Material::RefreshShader()
	{
		m_Shader->Reload();
		return true;
	}

	void Material::SetAlbedoMap(AssetHandle handle)
	{
		Albedo.Handle = handle;
		Albedo.Texture = AssetManager::GetAsset<Texture2D>(handle);
	}

	void Material::SetMetallicMap(AssetHandle handle)
	{
		Metallic.Handle = handle;
		Metallic.Texture = AssetManager::GetAsset<Texture2D>(handle);
	}

	std::shared_ptr<Material> Material::Create(const std::string &name)
	{
		OGN_PROFILER_RENDERING();

		return std::make_shared<Material>(name);
	}

	std::shared_ptr<Material> Material::Create(const std::shared_ptr<Shader> &shader)
	{
		return std::make_shared<Material>(shader);
	}

}