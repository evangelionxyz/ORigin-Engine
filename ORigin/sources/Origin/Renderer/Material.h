#pragma once
#include "Shader.h"
#include "Texture.h"

#include <assimp\scene.h>
#include <assimp\postprocess.h>

namespace origin {

	class Material
	{
	public:
		Material() = default;
		Material(std::string name, const std::shared_ptr<Shader>& shader);

		const std::string& GetName() const { return m_Name; }

		bool RefreshShader();

		std::shared_ptr<Shader> m_Shader;
		static std::shared_ptr<Material> Create(const std::string& name, const std::shared_ptr<Shader>& shader);

		int m_SpotLightCount = 0;
		int m_PointLightCount = 0;

		std::vector<std::shared_ptr<Texture2D>> LoadTextures(const std::string& modelFilepath, aiMaterial* mat, aiTextureType type);

	private:
		std::string m_Name;
		std::vector<std::shared_ptr<Texture2D>> m_LoadedTextures;
	};
}
