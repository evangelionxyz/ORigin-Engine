#include "pch.h"
#include "Material.h"

namespace origin {

	Material::Material(std::string name) : m_Name(name)
	{
		OGN_CORE_INFO("MATERIAL: Material {} Created", name);
	}

	void Material::LoadShader(std::shared_ptr<Shader> shader)
	{
		m_ShaderFilepath = shader->GetFilepath();
		m_ShaderEnableSpirv = shader->IsSpirvEnabled();
		m_ShaderRecompileSpirv = shader->IsRecompilerSpirv();

		m_Shader = shader;

		m_ShaderLoaded = true;

		OGN_CORE_INFO("MATERIAL: Shader loaded {}", m_ShaderFilepath);
		OGN_CORE_INFO("          SPIRV {}", m_ShaderEnableSpirv);
		OGN_CORE_INFO("          RECOMPILE {}", m_ShaderRecompileSpirv);
	}

	void Material::LoadShader(const std::string& filepath, bool enableSpirv, bool recompile)
	{
		m_ShaderFilepath = filepath;
		m_Shader = Shader::Create(filepath, enableSpirv, recompile);

		m_ShaderEnableSpirv = enableSpirv;
		m_ShaderRecompileSpirv = recompile;

		m_ShaderLoaded = true;

		OGN_CORE_INFO("MATERIAL: Shader loaded {}", m_ShaderFilepath);
		OGN_CORE_INFO("          SPIRV {}", m_ShaderEnableSpirv);
		OGN_CORE_INFO("          RECOMPILE {}", m_ShaderRecompileSpirv);
	}

	bool Material::RefreshShader()
	{
		m_Shader.reset();

		m_Shader = Shader::Create(m_ShaderFilepath, m_ShaderEnableSpirv, m_ShaderRecompileSpirv);
		OGN_CORE_WARN("MATERIAL: Shader {} Refreshed", m_Shader->GetName());

		return true;
	}

	void Material::EnableShader()
	{
		m_Shader->Enable();
	}

	void Material::DisableShader()
	{
		m_Shader->Disable();
	}

	void Material::SetFloat(const std::string& name, float value)
	{
		m_Shader->SetFloat(name, value);
	}

	void Material::SetInt(const std::string& name, int value)
	{
		m_Shader->SetInt(name, value);
	}

	void Material::SetBool(const std::string& name, bool condition)
	{
		m_Shader->SetBool(name, condition);
	}

	void Material::SetVector(const std::string& name, const glm::vec2& vectors)
	{
		m_Shader->SetVector(name, vectors);
	}

	void Material::SetVector(const std::string& name, const glm::vec3& vectors)
	{
		m_Shader->SetVector(name, vectors);
	}

	void Material::SetVector(const std::string& name, const glm::vec4& vectors)
	{
		m_Shader->SetVector(name, vectors);
	}

	void Material::SetMatrix(const std::string& name, const glm::mat2& matrices)
	{
		m_Shader->SetMatrix(name, matrices);
	}

	void Material::SetMatrix(const std::string& name, const glm::mat3& matrices)
	{
		m_Shader->SetMatrix(name, matrices);
	}

	void Material::SetMatrix(const std::string& name, const glm::mat4& matrices)
	{
		m_Shader->SetMatrix(name, matrices);
	}

	std::vector<std::shared_ptr<Texture2D>> Material::LoadTextures(const std::string& modelFilepath, aiMaterial* mat, aiTextureType type, std::string typeName)
	{
		std::vector<std::shared_ptr<Texture2D>> returnTextures;

		for (uint32_t i = 0; i < mat->GetTextureCount(type); i++)
		{
			HasTexture = true;

			aiString str;

			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (uint32_t j = 0; j < m_LoadedTextures.size(); j++)
			{
				OGN_CORE_INFO("MODEL: Loaded: {}, Trying to Load: {}", m_LoadedTextures[j]->GetName(), str.C_Str());
				if (std::strcmp(m_LoadedTextures[j]->GetName().data(), str.C_Str()) == 0)
				{
					returnTextures.push_back(m_LoadedTextures[j]);
					skip = true;
					break;
				}
			}

			if (!skip)
			{
				auto& textureDirectory = modelFilepath.substr(0, modelFilepath.find_last_of('/'));
				std::string textureName = std::string(str.C_Str());

				std::shared_ptr<Texture2D> newTexture = Texture2D::Create(textureDirectory + "/" + textureName);
				newTexture->SetMaterialTypeName(typeName);
				returnTextures.push_back(newTexture);
				m_LoadedTextures.push_back(newTexture);
			}
		}
		return returnTextures;
	}

	std::shared_ptr<Material> Material::Create(const std::string& name)
	{
		return std::make_shared<Material>(name);
	}

}