// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "Bone.hpp"

namespace origin {

AnimationNode::AnimationNode(const aiNodeAnim *anim_node)
    : local_transform(1.0f)
{
    for (u32 positionIndex = 0; positionIndex < anim_node->mNumPositionKeys; ++positionIndex)
    {
        const aiVector3D aiPos = anim_node->mPositionKeys[positionIndex].mValue;
        const f32 anim_time = static_cast<f32>(anim_node->mPositionKeys[positionIndex].mTime);
        translation_keys.AddFrame({ Math::AssimpToGlmVec3(aiPos), anim_time });
    }

    for (u32 rotationIndex = 0; rotationIndex < anim_node->mNumRotationKeys; ++rotationIndex)
    {
        const aiQuaternion aiQuat = anim_node->mRotationKeys[rotationIndex].mValue;
        const f32 anim_time = static_cast<f32>(anim_node->mRotationKeys[rotationIndex].mTime);
        rotation_keys.AddFrame({ Math::AssimpToGlmQuat(aiQuat), anim_time });
    }

    for (u32 scaleIndex = 0; scaleIndex < anim_node->mNumScalingKeys; ++scaleIndex)
    {
        const aiVector3D aiScale = anim_node->mScalingKeys[scaleIndex].mValue;
        const f32 anim_time = static_cast<f32>(anim_node->mScalingKeys[scaleIndex].mTime);
        scale_keys.AddFrame({ Math::AssimpToGlmVec3(aiScale), anim_time });
    }
}

void AnimationNode::Update(f32 anim_time)
{
    const glm::mat4 translation = translation_keys.InterpolateTranslation(anim_time);
    const glm::mat4 rotation = rotation_keys.Interpolate(anim_time);
    const glm::mat4 scale = scale_keys.InterpolateScaling(anim_time);

    local_transform = translation * rotation * scale;
}

}
