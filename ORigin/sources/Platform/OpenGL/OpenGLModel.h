// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin/Renderer/Model.h"
#include "Origin/Renderer/UniformBuffer.h"
#include "Origin/Scene/Components/Components.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>

namespace origin
{
	class StaticMeshComponent;
	class OpenGLModel : public Model
	{
	public:
		OpenGLModel(const char *filepath, Scene *scene);
		~OpenGLModel() override;
		virtual const std::string &GetFilepath() override { return m_Filepath; }
		virtual const std::vector<StaticMeshComponent> &GetMeshes() override { return m_Meshes; }

	private:
		std::shared_ptr<UniformBuffer> m_UniformBuffer;
		std::vector<StaticMeshComponent> m_Meshes;
		
		void ProcessNode(const aiNode* node, const aiScene* scene);
		
		std::string m_Filepath;
		Scene *m_Scene = nullptr;
	};
}
