#pragma once
#include "Origin/Asset/Asset.h"

#include <vector>
#include <glm/glm.hpp>

namespace origin
{
    class VertexBuffer;
}

namespace origin
{
    class VertexArray;

    struct MeshVertexData
    {
        glm::vec3 Position;
        glm::vec3 Normals;
        glm::vec3 Color;
        glm::vec2 UV;
        glm::vec2 TilingFactor;
        float AlbedoIndex = 0.0f;
        float SpecularIndex = 0.0f;
        int EntityID = -1;
    };

    struct AnimatedMeshVertexData
    {
        glm::vec3 Position;
        glm::vec3 Normals;
        glm::vec2 UV;
        glm::vec3 BoneIDs;
        glm::vec4 BoneWeights;
        int EntityID = -1;
    };

    struct Bone
    {
        int ID = 0;
        std::string Name;
        glm::mat4 Offset;
        std::vector<Bone> Children;
    };

    struct BoneTransformTrack
    {
        // Transform Timestamp
        std::vector<float> PositionTs;
        std::vector<float> RotationTs;
        std::vector<float> ScaleTs;

        std::vector<glm::vec3> Positions;
        std::vector<glm::quat> Rotations;
        std::vector<glm::vec3> Scales;
    };

    struct Animation
    {
        float Duration = 0.0f;
        float TickPersecond = 1.0f;
        std::unordered_map<std::string, BoneTransformTrack> BoneTransforms;
    };

    class MeshData : public Asset
    {
    public:
        std::vector<MeshVertexData> vertices;
        std::vector<uint32_t> indices;
        std::vector<Animation> animations;
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<VertexBuffer> vertexBuffer;

        Bone bone;
        uint32_t numBones = 0;

        static AssetType GetStaticType() { return AssetType::Mesh; }
        virtual AssetType GetType() const { return GetStaticType(); }
    };

    class AnimatedMeshData: public Asset
    {
    public:
        std::vector<AnimatedMeshVertexData> vertices;
        std::vector<uint32_t> indices;
        std::vector<Animation> animations;
        std::shared_ptr<VertexArray> vertexArray;
        std::shared_ptr<VertexBuffer> vertexBuffer;

        Bone bone;
        uint32_t numBones = 0;

        static AssetType GetStaticType() { return AssetType::Mesh; }
        virtual AssetType GetType() const { return GetStaticType(); }
    };
}

