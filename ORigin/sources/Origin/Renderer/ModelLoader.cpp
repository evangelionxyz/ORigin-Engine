// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ModelLoader.h"

namespace origin
{
	inline glm::mat4 AssimpToGlmMatrix(aiMatrix4x4 mat)
	{
		glm::mat4 m;
		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				m[x][y] = mat[y][x];
			}
		}
		return m;
	}

	inline glm::vec3 AssimpToGlmVec3(aiVector3D vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	inline glm::quat AssimpToGlmQuat(aiQuaternion quat)
	{
		glm::quat q;
		q.x = quat.x;
		q.y = quat.y;
		q.z = quat.z;
		q.w = quat.w;
		return q;
	}

	static void LoadAnimation(const aiScene *scene, Animation *animation)
	{
		aiAnimation *anim = scene->mAnimations[0];
        animation->TickPersecond = anim->mTicksPerSecond;
		if (anim->mTicksPerSecond == 0.0f)
		{
			animation->TickPersecond = 1.0f;
		}

		animation->Duration = anim->mDuration;
		animation->BoneTransforms = {};

		for (uint32_t i = 0; i < anim->mNumChannels; ++i)
		{
			aiNodeAnim *channel = anim->mChannels[i];
			BoneTransformTrack track;

			for (uint32_t j = 0; j < channel->mNumPositionKeys; ++j)
			{
				track.PositionTs.push_back(channel->mPositionKeys[j].mTime);
				track.Positions.push_back(AssimpToGlmVec3(channel->mPositionKeys[j].mValue));
			}

            for (uint32_t j = 0; j < channel->mNumRotationKeys; ++j)
            {
                track.RotationTs.push_back(channel->mRotationKeys[j].mTime);
                track.Rotations.push_back(AssimpToGlmQuat(channel->mRotationKeys[j].mValue));
            }

            for (uint32_t j = 0; j < channel->mNumScalingKeys; ++j)
            {
                track.ScaleTs.push_back(channel->mScalingKeys[j].mTime);
                track.Scales.push_back(AssimpToGlmVec3(channel->mScalingKeys[j].mValue));
            }

			animation->BoneTransforms[channel->mNodeName.C_Str()] = track;
		}
	}

	static bool ReadSkeleton(Bone *bone, const aiNode *node, 
		std::unordered_map<std::string, std::pair<int, glm::mat4>> &boneInfo)
	{
		if (boneInfo.find(node->mName.C_Str()) != boneInfo.end())
		{
			bone->Name = node->mName.C_Str();
			bone->ID = boneInfo[bone->Name].first;
			bone->Offset = boneInfo[bone->Name].second;

			for (uint32_t i = 0; i < node->mNumChildren; ++i)
			{
				Bone child;
				ReadSkeleton(&child, node->mChildren[i], boneInfo);
				bone->Children.push_back(child);
			}

			return true;
		}
		else
		{
			for (uint32_t i = 0; i < node->mNumChildren; ++i)
			{
				if (ReadSkeleton(bone, node->mChildren[i], boneInfo))
				{
					return true;
				}
			}
		}

		return false;
	}

	static void ProcessNode(aiNode *node, const aiScene *scene, MeshData *data)
	{
		for (uint32_t meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[meshIndex]];

			// Vertices
			for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
			{
				MeshVertexData vertex;
				vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
				vertex.Normals = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
				vertex.UV = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
				vertex.TilingFactor	 = glm::vec2(1.0f);
				vertex.BoneIDs = glm::vec4(0.0f);
				vertex.BoneWeights = glm::vec4(0.0f);
				data->vertices.push_back(vertex);
			}

			// Bones
			std::unordered_map<std::string, std::pair<int, glm::mat4>> boneInfo;
			std::vector<uint32_t> boneCounts;
			boneCounts.resize(data->vertices.size(), 0);
			data->numBones = mesh->mNumBones;

			for (uint32_t i = 0; i < data->numBones; ++i)
			{
				aiBone *bone = mesh->mBones[i];
				glm::mat4 offsetMatrix = AssimpToGlmMatrix(bone->mOffsetMatrix);
				boneInfo[bone->mName.C_Str()] = { i, offsetMatrix };

				for (uint32_t j = 0; j < bone->mNumWeights; j++)
				{
					uint32_t id = bone->mWeights[j].mVertexId;
					float weight = bone->mWeights[j].mWeight;
					boneCounts[id]++;

					switch (boneCounts[id])
					{
					case 1:
						data->vertices[id].BoneIDs.x = i;
						data->vertices[id].BoneWeights.x = weight;
						break;
                    case 2:
                        data->vertices[id].BoneIDs.y = i;
                        data->vertices[id].BoneWeights.y = weight;
                        break;
                    case 3:
                        data->vertices[id].BoneIDs.z = i;
                        data->vertices[id].BoneWeights.z = weight;
                        break;
                    case 4:
                        data->vertices[id].BoneIDs.z = i;
                        data->vertices[id].BoneWeights.z = weight;
                        break;
					}
				}
			}


			// Normalize weights
			for(size_t i = 0; i < data->vertices.size(); ++i)
			{
				glm::vec4 &boneWeights = data->vertices[i].BoneWeights;
				float totalWeight = boneWeights.x + boneWeights.y + boneWeights.z + boneWeights.w;

				if (totalWeight > 0.0f)
				{
					data->vertices[i].BoneWeights = {
						boneWeights.x / totalWeight,
						boneWeights.y / totalWeight,
						boneWeights.z / totalWeight,
						boneWeights.w / totalWeight
					};
				}
			}

            // Faces
            for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
            {
                aiFace face = mesh->mFaces[i];
                for (uint32_t in = 0; in < face.mNumIndices; in++)
                {
                    data->indices.push_back(face.mIndices[in]);
                }
            }

			ReadSkeleton(&data->bone, node, boneInfo);
		}

        for (uint32_t i = 0; i < node->mNumChildren; ++i)
        {
            ProcessNode(node->mChildren[i], scene, data);
        }
	}

	MeshData ModelLoader::LoadModel(const char *filepath)
	{
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(filepath,
			  aiProcess_Triangulate | aiProcess_GenSmoothNormals
			| aiProcess_FlipUVs     | aiProcess_JoinIdenticalVertices
		);

		aiNode *node = scene->mRootNode;

		MeshData data;
		ProcessNode(node, scene, &data);

		if (scene->mNumAnimations)
		{
			for (uint32_t i = 0; i < scene->mNumAnimations; ++i)
			{
				Animation anim;
				LoadAnimation(scene, &anim);
				data.animations.push_back(anim);
			}
		}

		return data;
	}

	void ModelLoader::ProcessMesh(const MeshData &data, std::shared_ptr<VertexArray> &vertexArray, std::shared_ptr<VertexBuffer> &vertexBuffer)
	{
		vertexArray = VertexArray::Create();
		vertexBuffer = VertexBuffer::Create((void *)data.vertices.data(), data.vertices.size() * sizeof(MeshVertexData));

		vertexBuffer->SetLayout
		({
			{ ShaderDataType::Float3, "aPosition"     },
			{ ShaderDataType::Float3, "aNormals"      },
			{ ShaderDataType::Float3, "aColor"        },
			{ ShaderDataType::Float2, "aUV"           },
			{ ShaderDataType::Float2, "aTilingFactor" },
			{ ShaderDataType::Float4, "aBoneIDs"      },
			{ ShaderDataType::Float4, "aBoneWeights"  },
			{ ShaderDataType::Float,  "aAlbedoIndex"  },
			{ ShaderDataType::Float,  "aSpecularIndex"},
			{ ShaderDataType::Int,	  "aEntityID"     }
		});

		vertexArray->AddVertexBuffer(vertexBuffer);

		std::shared_ptr<IndexBuffer> indexBuffer = IndexBuffer::Create(data.indices);
		vertexArray->SetIndexBuffer(indexBuffer);

		OGN_CORE_WARN("INDEX COUNT: {}", indexBuffer->GetCount());
	}

}