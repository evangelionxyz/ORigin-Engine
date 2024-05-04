// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\Model.h"
#include "Origin\Renderer\Mesh.h"
#include "Origin\Renderer\UniformBuffer.h"
#include "Origin\Renderer\Texture.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <vector>

#include "Origin\Renderer\Material.h"

namespace origin
{
	class OpenGLModel : public Model
	{
	public:
		OpenGLModel() = default;
		OpenGLModel(const std::filesystem::path & filepath);

		~OpenGLModel() override;
		void Draw() override;
		void Draw(int entityID) override;
		void SetTransform(const glm::mat4& transform) override;
		void SetMaterial(std::shared_ptr<Material> mat) override;
		void RemoveMaterial() override;

		std::shared_ptr<Material>& GetMaterial() override { return m_Material; }
		const std::filesystem::path &GetFilepath() const override { return m_Filepath; }

	private:
		struct ModelBufferData
		{
			glm::mat4 Transform;
			int EntityID;
		};
		ModelBufferData m_ModelBufferData;

		std::vector<MeshVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<std::shared_ptr<Mesh>> m_Meshes;
		std::shared_ptr<UniformBuffer> m_Uniformbuffer;

		void ProcessNode(aiNode* node, const aiScene* scene);
		std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

		std::shared_ptr<Material> m_Material;
		std::filesystem::path m_Filepath;

		std::vector<std::unordered_map<aiTextureType, std::shared_ptr<Texture2D>>> m_Textures;
	};
}
