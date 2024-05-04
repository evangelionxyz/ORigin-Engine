// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Material.h"
#include "Origin\Asset\AssetManager.h"

namespace origin {

	Material::Material()
	{
		OGN_PROFILER_RENDERING();

		m_Shader = Shader::Create("Resources/Shaders/SPIR-V/Mesh.glsl", true);
		m_Shader->Enable();

		int binding = 4;
		m_UniformBuffer = UniformBuffer::Create(sizeof(MaterialBufferData), binding);
	}

	Material::Material(const std::shared_ptr<Shader> &shader)
		: m_Shader(shader)
	{
		OGN_PROFILER_RENDERING();

		OGN_CORE_ASSERT(m_Shader, "[Material] Shader is uninitialized or empty!")
		m_Shader->Enable();

		int binding = 4;
		m_UniformBuffer = UniformBuffer::Create(sizeof(MaterialBufferData), binding);
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
		OGN_PROFILER_RENDERING();

		std::string shaderPath = m_Shader->GetFilepath();
		bool enableSpirv = m_Shader->IsSpirvEnabled();
		bool recompile = m_Shader->IsRecompilerSpirv();

		m_Shader.reset();

		m_Shader = Shader::Create(shaderPath, enableSpirv, recompile);
		OGN_CORE_WARN("[Material] Shader {} Refreshed", m_Shader->GetName());

		return true;
	}

	std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>> Material::LoadTextures(const std::string& modelFilepath, aiMaterial* mat, aiTextureType type)
	{
		OGN_PROFILER_RENDERING();

		std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>> textures;

		for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
		{
			OGN_PROFILER_SCOPE("Material::LoadTextures TextureCount");

			aiString str;

			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (uint32_t j = 0; j < m_LoadedTextures.size(); j++)
			{
				if (std::strcmp(m_LoadedTextures[j]->GetName().c_str(), str.C_Str()) == 0)
				{
					textures[type] = m_LoadedTextures[j];
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				OGN_PROFILER_SCOPE("Material::LoadTextures TextureCount");

				auto& textureDirectory = modelFilepath.substr(0, modelFilepath.find_last_of('/'));
				std::string textureName = std::string(str.C_Str());

				std::shared_ptr<Texture2D> newTexture = Texture2D::Create(textureDirectory + "/" + textureName);
				textures[type] = newTexture;
				m_LoadedTextures.push_back(newTexture);
			}
		}

		return textures;
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