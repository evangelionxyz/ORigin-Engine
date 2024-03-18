// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Asset\Asset.h"
#include "Shader.h"
#include "Texture.h"
#include "UniformBuffer.h"

#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <unordered_map>

namespace origin {

	struct MaterialBufferData
	{
		glm::vec4 Color = glm::vec4(1.0f);
		float Emission = 0.0f;
		float MetallicValue = 0.0f;
		float RoughnessValue = 0.0f;
		bool UseNormalMaps = false;
	};

	class Material : public Asset
	{
	public:
		Material();
		Material(const std::shared_ptr<Shader> &shader);
		MaterialBufferData BufferData;

		void Bind();
		void Unbind();
		void AddShader(const std::shared_ptr<Shader> &shader);

		bool RefreshShader();
		std::shared_ptr<Shader> m_Shader;
		static std::shared_ptr<Material> Create();
		static std::shared_ptr<Material> Create(const std::shared_ptr<Shader> &shader);
		AssetHandle AlbedoMap;

		static AssetType GetStaticType() { return AssetType::Material; }
		virtual AssetType GetType() const { return GetStaticType(); }

	private:
		std::shared_ptr<UniformBuffer> m_UniformBuffer;
		std::vector<std::shared_ptr<Texture2D>> m_LoadedTextures;
		std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>> LoadTextures(const std::string& modelFilepath, aiMaterial* mat, aiTextureType type);

		friend class OpenGLModel;
	};
}
