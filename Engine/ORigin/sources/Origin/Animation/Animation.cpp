// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Animation.h"

namespace origin {

AnimationNode::AnimationNode(const aiNodeAnim *anim_node)
    : transform(1.0f), translation(0.0f), scale(1.0f), rotation({ 1.0f, 0.0f, 0.0f, 0.0f })
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
    translation = translation_keys.InterpolateTranslation(anim_time);
    rotation = rotation_keys.InterpolateRotation(anim_time);
    scale = scale_keys.InterpolateScaling(anim_time);

    transform = glm::translate(glm::mat4(1.0f), translation)
        * glm::toMat4(rotation)
        * glm::scale(glm::mat4(1.0f), scale);
}

SkeletalAnimation::SkeletalAnimation(aiAnimation *anim)
    : m_Anim(anim)
{
    m_Name = anim->mName.C_Str();

    modf(static_cast<f32>(anim->mDuration), &m_Duration);

    m_TicksPerSecond = static_cast<f32>(anim->mTicksPerSecond);
    if (anim->mTicksPerSecond == 0.0) m_TicksPerSecond = 25.0f;

    ReadChannels(anim);
}

void SkeletalAnimation::UpdateTime(f32 delta_time, f32 speed)
{
    m_TimeInSeconds += delta_time * speed;
    m_TimeInTicks = m_TimeInSeconds * m_TicksPerSecond;
    m_TimeInTicks = fmod(m_TimeInTicks, m_Duration);
}

void SkeletalAnimation::ReadChannels(const aiAnimation *anim)
{
    for (u32 i = 0; i < anim->mNumChannels; ++i)
    {
        aiNodeAnim *node_anim = anim->mChannels[i];
        std::string node_name(node_anim->mNodeName.C_Str());
        m_ChannelMap[node_name] = AnimationNode(node_anim);
    }
}
}