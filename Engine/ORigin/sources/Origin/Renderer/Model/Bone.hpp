// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef BONE_HPP
#define BONE_HPP

#include "Origin/Animation/KeyFrame.hpp"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace origin {

#define MAX_NUM_BONES_PER_VERTEX (4)

struct VertexBoneData
{
    VertexBoneData() = default;

    void AddBoneData(u32 bone_id, f32 weight)
    {
        for (u32 i = 0; i < std::size(bone_ids); ++i)
        {
            if (weights[i] == 0.0f)
            {
                bone_ids[i] = bone_id;
                weights[i] = weight;
                return;
            }
        }
    }

    u32 bone_ids[MAX_NUM_BONES_PER_VERTEX] = { 0 };
    f32 weights[MAX_NUM_BONES_PER_VERTEX] = { 0.0f };
};


class Bone
{
public:
    Bone() = default;
    Bone(const std::string &name, i32 id, const aiNodeAnim *anim_node);

    void Update(f32 time);

    Vec3Key translation_keys;
    QuatKey rotation_keys;
    Vec3Key scale_keys;

    i32 id;
    std::string name;
    glm::mat4 offset_matrix;
    glm::mat4 local_transform;
    glm::mat4 global_transform;

    std::vector<std::string> children;
};

}

#endif