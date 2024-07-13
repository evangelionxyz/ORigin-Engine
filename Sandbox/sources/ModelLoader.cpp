#include "ModelLoader.h"

using namespace origin;

static void ProcessNode(aiNode *node, const aiScene *scene, MeshData *data)
{
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *aiMesh = scene->mMeshes[node->mMeshes[i]];

		for (uint32_t i = 0; i < aiMesh->mNumVertices; i++)
		{
			MeshVertexData vertex;
			vertex.Position = { aiMesh->mVertices[i].x, aiMesh->mVertices[i].y, aiMesh->mVertices[i].z };

			if (aiMesh->HasNormals())
			{
				vertex.Normals = { aiMesh->mNormals[i].x, aiMesh->mNormals[i].y, aiMesh->mNormals[i].z };
			}

			if (aiMesh->mTextureCoords[0])
				vertex.TexCoord = { aiMesh->mTextureCoords[0][i].x, aiMesh->mTextureCoords[0][i].y };
			else
				vertex.TexCoord = { 0.0f, 0.0f };

			data->vertices.push_back(vertex);
		}

		for (uint32_t f = 0; f < aiMesh->mNumFaces; f++)
		{
			aiFace face = aiMesh->mFaces[f];
			for (uint32_t in = 0; in < face.mNumIndices; in++)
				data->indices.push_back(face.mIndices[in]);
		}
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, data);
	}
}

MeshData ModelLoader::LoadModel(const char *filepath)
{
	Assimp::Importer importer;
	const aiScene *aiScene = importer.ReadFile(filepath,
			aiProcess_Triangulate | aiProcess_GenSmoothNormals
			| aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
	);

	aiNode *node = aiScene->mRootNode;
	MeshData data;
	ProcessNode(node, aiScene, &data);

	return data;
}

void ModelLoader::ProcessMesh(const MeshData &data, std::shared_ptr<VertexArray> &vertexArray, std::shared_ptr<VertexBuffer> &vertexBuffer)
{
	vertexArray = VertexArray::Create();
	vertexBuffer = VertexBuffer::Create((void *)data.vertices.data(), data.vertices.size() * sizeof(MeshVertex));

	vertexBuffer->SetLayout
	({
		{ ShaderDataType::Float3, "aPosition"		},
		{ ShaderDataType::Float3, "aNormals"		},
		{ ShaderDataType::Float4, "aColor"			},
		{	ShaderDataType::Float2,	"aTexCoord"		},
		{ ShaderDataType::Float,	"aTexIndex"   },
		{ ShaderDataType::Int,	"aEntityID"   }
	});

	vertexArray->AddVertexBuffer(vertexBuffer);

	std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(data.indices);
	vertexArray->SetIndexBuffer(indexBuffer);

	OGN_CORE_WARN("INDEX COUNT: {}", indexBuffer->GetCount());
}

