// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef BONE_HPP
#define BONE_HPP

#include "Origin/Animation/KeyFrame.hpp"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

namespace origin {

struct BoneInfo
{
    glm::mat4 offset_matrix;
    glm::mat4 final_transformation;

    BoneInfo(const glm::mat4 &offset)
    {
        offset_matrix = offset;
        final_transformation = glm::mat4(1.0f);
    }
};

class AnimationNode
{
public:
    AnimationNode() = default;
    AnimationNode(const aiNodeAnim *anim_node);

    void Update(f32 time);

    Vec3Key translation_keys;
    QuatKey rotation_keys;
    Vec3Key scale_keys;

    glm::mat4 local_transform;
    glm::vec3 translation;
    glm::vec3 scale;
    glm::quat rotation;
};

}

#endif