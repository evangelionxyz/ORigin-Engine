// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Renderer\Model.h"
#include "Origin\Renderer\Mesh.h"
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
		OpenGLModel(const std::string& filepath, std::shared_ptr<Material> material);

		virtual ~OpenGLModel() override;
		void Draw() override;
		void Draw(const glm::mat4& modelTransform, const EditorCamera& camera, int entityID) override;
		const std::string& GetFilepath() const override { return m_Filepath; }

	private:
		std::vector<MeshVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::vector<std::shared_ptr<Mesh>> m_Meshes;

		void ProcessNode(aiNode* node, const aiScene* scene);
		std::shared_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);

		std::shared_ptr<Material> m_Material;
		std::string m_Filepath;
	};
}


