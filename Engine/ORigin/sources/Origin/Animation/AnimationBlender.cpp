// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "AnimationBlender.h"
#include "Animation.h"
#include "Origin/Renderer/Model/Model.hpp"
#include <numeric>

namespace origin {

void AnimationBlender::SetModel(Ref<Model> &model)
{
    m_Model = model;
    m_Model->m_FinalBoneTransforms.resize(m_Model->m_BoneInfo.size());

    std::function<void(aiNode *)> ReadNodeFunc = [&](aiNode * node)
    {
        m_AiNodes.push_back(node);
        for (u32 i = 0; i < node->mNumChildren; ++i)
        {
            ReadNodeFunc(node->mChildren[i]);
        }
    };
}

void AnimationBlender::SetRange(const glm::vec2 &min_size, const glm::vec2 &max_size)
{
    m_MinSize = min_size;
    m_MaxSize = max_size;
}

void AnimationBlender::AddAnimation(i32 anim_index, const glm::vec2 &min_range, const glm::vec2 &max_range)
{
    glm::vec2 pos = glm::clamp(min_range, m_MinSize, m_MaxSize);
    std::string name = m_Model->m_Animations[anim_index].GetName();
    m_States.push_back({ name, min_range, max_range, anim_index, 0.0f, {} });
}

void AnimationBlender::BlendAnimations(const glm::vec2 &current_position, f32 delta_time, const f32 speed)
{
    std::unordered_set<int> updated_animations;

    // First pass: Update animation times
    for (auto &state : m_States)
    {
        state.weight = CalculateWeightForRange(current_position, state.min_range, state.max_range);

        // check if this animation has already been updated
        if (!updated_animations.contains(state.anim_index))
        {
            updated_animations.insert(state.anim_index); // mark as updated

            SkeletalAnimation &anim = m_Model->GetAnimations()[state.anim_index];
            anim.UpdateTime(delta_time, speed);
        }
        m_Model->CalculateAnimationTransforms(m_Model->m_Scene->mRootNode, state.anim_index, state.anim_nodes);
    }

    // Third pass: Blend the bone transforms
    for (auto &[bone_name, bone_index] : m_Model->m_BoneNameToIndexMap)
    {
        glm::vec3 blended_translation(0.0f);
        glm::quat blended_orientation(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 blended_scale(0.0f);

        glm::vec3 blended_parent_translation(0.0f);
        glm::quat blended_parent_orientation(0.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 blended_parent_scale(0.0f);

        f32 total_weight = 0.0f;

        for (auto &state : m_States)
        {
            updated_animations.insert(state.anim_index); // mark as updated

            total_weight += state.weight;

            glm::vec3 translation = state.anim_nodes[bone_name].translation;
            glm::quat orientation = state.anim_nodes[bone_name].rotation;
            glm::vec3 scale = state.anim_nodes[bone_name].scale;

            blended_translation += state.weight * translation;
            blended_orientation = glm::slerp(blended_orientation, orientation, state.weight); // Weighted slerp for orientation
            blended_scale += state.weight * scale;

            glm::vec4 perspective;
            glm::vec3 parent_translation, parent_scale, skew;
            glm::quat parent_orientation;
            glm::decompose(state.anim_nodes[bone_name].parent_transform, parent_scale, parent_orientation, parent_translation, skew, perspective);

            blended_parent_translation += state.weight * parent_translation;
            blended_parent_orientation = glm::slerp(blended_parent_orientation, parent_orientation, state.weight);
            blended_parent_scale += state.weight * parent_scale;
        }

        if (total_weight > 0.0f)
        {
            blended_translation /= total_weight;
            blended_scale /= total_weight;

            blended_orientation = glm::normalize(blended_orientation);
            blended_parent_orientation = glm::normalize(blended_parent_orientation);

            blended_parent_translation /= total_weight;
            blended_parent_scale /= total_weight;
        }

        glm::mat4 parent_transform = glm::translate(glm::mat4(1.0f), blended_parent_translation) * glm::mat4_cast(blended_parent_orientation) * glm::scale(glm::mat4(1.0f), blended_parent_scale);
        glm::mat4 node_transform = glm::translate(glm::mat4(1.0f), blended_translation) * glm::mat4_cast(blended_orientation) * glm::scale(glm::mat4(1.0f), blended_scale);
        m_Model->m_FinalBoneTransforms[bone_index] = parent_transform * node_transform * m_Model->m_BoneInfo[bone_index].offset_matrix;
    }
}

f32 AnimationBlender::CalculateWeightForRange(const glm::vec2 &current_pos, const glm::vec2 &min_range, const glm::vec2 &max_range)
{
    glm::vec2 clamped_pos = glm::clamp(current_pos, min_range, max_range);

    glm::vec2 range_size = max_range - min_range;
    glm::vec2 normalized_pos = (clamped_pos - min_range) / range_size;

    glm::vec2 center = (max_range + min_range) * 0.5f;
    glm::vec2 distance_from_center = glm::abs(current_pos - center) / (max_range - min_range);

    f32 normalized_distance = glm::length(distance_from_center);
    return std::max(0.0f, 1.0f - normalized_distance);
}

}

