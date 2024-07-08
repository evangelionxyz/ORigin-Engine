// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Renderer/Model.h"
#include "Origin/Renderer/Mesh.h"
#include "Origin/Renderer/UniformBuffer.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/Material.h"
#include "Origin/Scene/Components.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>

namespace origin
{
	class StaticMeshComponent;
	class OpenGLModel : public Model
	{
	public:
		OpenGLModel(const std::filesystem::path & filepath, Scene *scene);
		~OpenGLModel() override;

		void SetTransform(const glm::mat4 &transform) override;
		void SetMaterial(AssetHandle handle) override;
		const std::filesystem::path &GetFilepath() const override { return m_Filepath; }
		std::vector<UUID> GetMeshes() override { return m_MeshEntities; }
		AssetHandle GetMaterial() override;

	private:
		std::vector<MeshVertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
		std::shared_ptr<UniformBuffer> m_Uniformbuffer;
		std::vector<UUID> m_MeshEntities;

		void ProcessNode(aiNode* node, const aiScene* scene);
		std::shared_ptr<Mesh> ProcessMesh(aiMesh *mesh, const aiScene *scene);
		std::filesystem::path m_Filepath;
		Scene *m_Scene = nullptr;
	};
}
