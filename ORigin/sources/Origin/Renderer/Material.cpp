// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"

#include "Material.h"
#include "Origin/Asset/AssetManager.h"

#include "Renderer.h"

namespace origin {

#define ALBEDO_MAP		"u_AlbedoMap"
#define SPECULAR_MAP	"u_SpecularMap"

	Material::Material()
	{
		OGN_PROFILER_RENDERING();

		m_Shader = Shader::Create("Resources/Shaders/SPIR-V/Mesh.glsl", true);
		m_Shader->Enable();

		int binding = 1;
		m_UniformBuffer = UniformBuffer::Create(sizeof(MaterialBufferData), binding);
	}

	Material::Material(const std::shared_ptr<Shader> &shader)
		: m_Shader(shader)
	{
		OGN_PROFILER_RENDERING();

		OGN_CORE_ASSERT(m_Shader, "[Material] Shader is uninitialized or empty!")
		m_Shader->Enable();

		int binding = 1;
		m_UniformBuffer = UniformBuffer::Create(sizeof(MaterialBufferData), binding);
	}

	void Material::Bind()
	{
		OGN_PROFILER_RENDERING();

		m_UniformBuffer->Bind();
		m_UniformBuffer->SetData(&BufferData, sizeof(MaterialBufferData));
		m_Shader->Enable();

		if (Textures.empty() && (m_AlbedoMap == 0 && m_MetallicMap == 0))
		{
			Renderer::WhiteTexture->Bind(0);
			Renderer::WhiteTexture->Bind(1);
		}

		if (m_AlbedoMap == 0 && m_MetallicMap == 0)
		{
			for (auto &t : Textures)
			{
				if (t.first == aiTextureType_DIFFUSE)
				{
					t.second->Bind(0);
					m_Shader->SetInt(ALBEDO_MAP, 0);
				}
				else if (t.first == aiTextureType_SPECULAR)
				{
					t.second->Bind(1);
					m_Shader->SetInt(ALBEDO_MAP, 1);
				}
			}
		}

		if (m_AlbedoMap != 0)
			AssetManager::GetAsset<Texture2D>(m_AlbedoMap)->Bind(0);

		if (m_MetallicMap != 0)
			AssetManager::GetAsset<Texture2D>(m_MetallicMap)->Bind(1);

		m_Shader->SetInt(ALBEDO_MAP, 0);
		m_Shader->SetInt(SPECULAR_MAP, 1);
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
		m_AlbedoMap = handle;
	}

	void Material::SetMetallicMap(AssetHandle handle)
	{
		m_MetallicMap = handle;
	}

	std::shared_ptr<Material> Material::Create()
	{
		OGN_PROFILER_RENDERING();

		return std::make_shared<Material>();
	}

	std::shared_ptr<Material> Material::Create(const std::shared_ptr<Shader> &shader)
	{
		return std::make_shared<Material>(shader);
	}

}