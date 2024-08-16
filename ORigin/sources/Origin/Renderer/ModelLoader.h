#pragma once

#include "MeshVertexData.h"
#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Buffer.h"

namespace origin
{
	struct ModelLoader
	{
		static std::shared_ptr<StaticMeshData> LoadStaticModel(const std::filesystem::path &filepath);
		static void ProcessStaticMesh(const std::shared_ptr<StaticMeshData> &data);

		static std::shared_ptr<MeshData> LoadModel(const std::filesystem::path &filepath);
        static void ProcessMesh(const std::shared_ptr<MeshData> &data);
		static void ExtractBoneWeightForVertices(MeshData *data, aiMesh *mesh, const aiScene *scene);
	};
}


