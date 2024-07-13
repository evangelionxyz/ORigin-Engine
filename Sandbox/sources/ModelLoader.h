#pragma once

#include "MeshVertexData.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Origin/Renderer/VertexArray.h"
#include "Origin/Renderer/Buffer.h"

using namespace origin;

struct MeshData
{
	std::vector<MeshVertexData> vertices;
	std::vector<uint32_t> indices;
};

struct ModelLoader
{
public:
	static MeshData LoadModel(const char *filepath);
	static void ProcessMesh(const MeshData &data, std::shared_ptr<VertexArray> &vertexArray, std::shared_ptr<VertexBuffer> &vertexBuffer);
};

