#pragma once

#include "MeshVertexData.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Buffer.h"

namespace origin
{
	struct ModelLoader
	{
	public:
		static std::shared_ptr<MeshData> LoadModel(const std::filesystem::path &filepath);
		static std::shared_ptr<AnimatedMeshData> LoadAnimatedModel(const std::filesystem::path &);

		static void ProcessMesh(const std::shared_ptr<MeshData> &data,
			std::shared_ptr<VertexArray> &vertexArray,
			std::shared_ptr<VertexBuffer> &vertexBuffer);


        static void ProcessAnimatedMesh(const std::shared_ptr<AnimatedMeshData> &data,
            std::shared_ptr<VertexArray> &vertexArray,
            std::shared_ptr<VertexBuffer> &vertexBuffer);
	};
}


