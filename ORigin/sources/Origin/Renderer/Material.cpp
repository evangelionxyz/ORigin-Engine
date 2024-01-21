#include "pch.h"
#include "Material.h"

namespace origin {

	Material::Material(std::string name, const std::shared_ptr<Shader> shader)
		: m_Name(name), m_Shader(shader)
	{
		m_UniformBuffer = UniformBuffer::Create(sizeof(MaterialBufferData), 4);
		OGN_CORE_INFO("MATERIAL: Material {} Created", name);
	}

	void Material::OnRender()
	{
		m_UniformBuffer->Bind();
		m_UniformBuffer->SetData(&BufferData, sizeof(MaterialBufferData));
	}

	bool Material::RefreshShader()
	{
		std::string shaderPath = m_Shader->GetFilepath();
		bool enableSpirv = m_Shader->IsSpirvEnabled();
		bool recompile = m_Shader->IsRecompilerSpirv();

		m_Shader.reset();

		m_Shader = Shader::Create(shaderPath, enableSpirv, recompile);
		OGN_CORE_WARN("MATERIAL: Shader {} Refreshed", m_Shader->GetName());

		return true;
	}

	void Material::LoadTexture(const char* filepath)
	{
		std::shared_ptr<Texture2D> texture = Texture2D::Create(filepath);
		m_LoadedTextures.push_back(texture);
	}

	std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>> Material::LoadTextures(const std::string& modelFilepath, aiMaterial* mat, aiTextureType type)
	{
		std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>> textures;

		for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
		{
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
				auto& textureDirectory = modelFilepath.substr(0, modelFilepath.find_last_of('/'));
				std::string textureName = std::string(str.C_Str());

				std::shared_ptr<Texture2D> newTexture = Texture2D::Create(textureDirectory + "/" + textureName);
				textures[type] = newTexture;
				m_LoadedTextures.push_back(newTexture);
			}
		}

		return textures;
	}

	std::shared_ptr<Material> Material::Create(const std::string& name, const std::shared_ptr<Shader> shader)
	{
		return std::make_shared<Material>(name, shader);
	}

}