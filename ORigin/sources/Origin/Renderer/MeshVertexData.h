#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace origin
{
    struct MeshVertexData
    {
        glm::vec3 Position;
        glm::vec3 Normals;
        glm::vec3 Color;
        glm::vec2 UV;
        glm::vec2 TilingFactor;
        glm::vec4 BoneIDs;
        glm::vec4 BoneWeights;
        float AlbedoIndex = 0.0f;
        float SpecularIndex = 0.0f;
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

    struct MeshData
    {
        std::vector<MeshVertexData> vertices;
        std::vector<uint32_t> indices;
        Bone bone;
        std::vector<Animation> animations;
        uint32_t numBones = 0;
    };
}

