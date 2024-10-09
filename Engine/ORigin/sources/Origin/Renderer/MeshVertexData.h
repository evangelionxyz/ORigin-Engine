// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef MESH_VERTEX_DATA_H
#define MESH_VERTEX_DATA_H

#include "Origin/Asset/Asset.h"
#include "Origin/Animation/ModelAnimation.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace origin
{
    class VertexArray;
    class VertexBuffer;
    class Texture2D;

    struct StaticMeshVertexData
    {
        glm::vec3 Position;
        glm::vec3 Normals;
        glm::vec3 Color;
        glm::vec2 UV;
        glm::vec2 TilingFactor;
        float AlbedoIndex = 0.0f;
        float SpecularIndex = 0.0f;
    };

    class StaticMeshData : public Asset
    {
    public:
        std::string Name;
        std::vector<StaticMeshVertexData> vertices;
        std::vector<uint32_t> indices;
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<VertexBuffer> vertexBuffer;
        std::vector<Ref<Texture2D>> textures;

        static AssetType GetStaticType() { return AssetType::StaticMesh; }
        virtual AssetType GetType() const { return GetStaticType(); }
    };

    // ================================
    // Skinned Mesh

#define MAX_BONE_INFLUENCE 4
    struct MeshVertexData
    {
        glm::vec3 Position;
        glm::vec3 Normals;
        glm::vec2 UV;
        int IDs[MAX_BONE_INFLUENCE];
        float Weights[MAX_BONE_INFLUENCE];
    };

    struct BoneInfo
    {
        int ID;
        glm::mat4 OffsetMatrix;
    };

    class MeshData : public Asset
    {
    public:
        std::string Name;
        std::vector<MeshVertexData> vertices;
        std::vector<uint32_t> indices;
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<VertexBuffer> vertexBuffer;
        std::shared_ptr<Texture2D> diffuseTexture;

        std::map<std::string, BoneInfo> boneInfoMap;
        int boneCount = 0;

        static AssetType GetStaticType() { return AssetType::Mesh; }
        virtual AssetType GetType() const { return GetStaticType(); }
    };
}

#endif