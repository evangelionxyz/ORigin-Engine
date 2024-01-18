#pragma once
#include "Shader.h"
#include "Texture.h"
#include "UniformBuffer.h"

#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <unordered_map>

namespace origin {

	class Material
	{
	public:
		Material() = default;
		Material(std::string name, const std::shared_ptr<Shader> shader);

		struct MaterialBufferData
		{
			glm::vec4 Color = glm::vec4(1.0f);
			glm::vec2 TilingFactor = glm::vec2(1.0f);
			float Metallic = 0.0f;
			float Roughness = 0.0f;
		};
		MaterialBufferData BufferData;

		void OnRender();
		bool RefreshShader();
		const std::string& GetName() const { return m_Name; }

		std::shared_ptr<Shader> m_Shader;
		std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>> LoadTextures(const std::string& modelFilepath, aiMaterial* mat, aiTextureType type);

		static std::shared_ptr<Material> Create(const std::string& name, const std::shared_ptr<Shader> shader);

		int m_SpotLightCount = 0;
		int m_PointLightCount = 0;
	private:
		std::string m_Name;
		std::vector<std::shared_ptr<Texture2D>> m_LoadedTextures;
		std::shared_ptr<UniformBuffer> m_UniformBuffer;
	};
}
