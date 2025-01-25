// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "AnimationBlender.h"
#include "Animation.h"
#include "Origin/Renderer/Model/Model.hpp"

#include <functional>

namespace origin {

void AnimationBlender::SetModel(Ref<Model> &model)
{
    m_Model = model;

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

    for (auto &state : m_States)
    {
        state.position = glm::clamp(state.position, min_size, max_size);
    }
}

void AnimationBlender::AddAnimation(i32 anim_index, const glm::vec2 &position)
{
    glm::vec2 pos = glm::clamp(position, m_MinSize, m_MaxSize);
    m_States.push_back({ anim_index, 0.0f, position, {} });
}

void AnimationBlender::BlendAnimations(const glm::vec2 &current_position, f32 delta_time, const f32 speed, f32 blend_factor)
{
    m_Model->m_FinalBoneTransforms.resize(m_Model->m_BoneInfo.size());
    std::vector<glm::mat4> blended_transforms(m_Model->m_BoneInfo.size(), glm::mat4(1.0f));
    
    for (auto &state : m_States)
    {
        state.weight = CalculateWeight(current_position, state.position);
    }

    f32 total_weight = 0.0f;
    std::vector<f32> normalized_weights;
    std::unordered_map<i32, f32> weight_map;

    for (auto &state : m_States)
    {
        f32 weight = state.weight;
        normalized_weights.push_back(weight);
        total_weight += weight;

        // accumulate weight per animation index
        weight_map[state.anim_index] += weight;

        SkeletalAnimation &anim = m_Model->GetAnimations()[state.anim_index];
        anim.UpdateTime(delta_time, speed);
        m_Model->CalculateAnimationTransforms(m_Model->m_Scene->mRootNode, state.anim_index, state.anim_nodes);
    }

    // normalize weights
    if (total_weight > 0.0f)
    {
        for (auto &pair : weight_map)
            pair.second /= total_weight;
    }

    // Blend animations
    i32 state_index = 0;
    std::unordered_set<i32> processed_anim_indices; // save processed animation indices for multiple animation position

    for (const auto &[bone_name, bone_index] : m_Model->m_BoneNameToIndexMap)
    {
        glm::vec3 translation_a = m_States[0].anim_nodes[bone_name].translation;
        glm::quat orientation_a = m_States[0].anim_nodes[bone_name].rotation;
        glm::vec3 scale_a = m_States[0].anim_nodes[bone_name].scale;

        glm::vec3 translation_b = m_States[1].anim_nodes[bone_name].translation;
        glm::quat orientation_b = m_States[1].anim_nodes[bone_name].rotation;
        glm::vec3 scale_b = m_States[1].anim_nodes[bone_name].scale;

        glm::vec3 blended_translation = glm::mix(translation_a, translation_b, blend_factor);
        glm::quat blended_orientation = glm::slerp(orientation_a, orientation_b, blend_factor);
        glm::vec3 blended_scale = glm::mix(scale_a, scale_b, blend_factor);

        glm::vec4 perspective;
        glm::vec3 translation_c, translation_d, scale_c, scale_d, skew;
        glm::quat orientation_c, orientation_d;

        glm::decompose(m_States[0].anim_nodes[bone_name].parent_transform, scale_c, orientation_c, translation_c, skew, perspective);
        glm::decompose(m_States[1].anim_nodes[bone_name].parent_transform, scale_d, orientation_d, translation_d, skew, perspective);

        glm::vec3 blended_parent_translation = glm::mix(translation_c, translation_d, blend_factor);
        glm::quat blended_parent_orientation = glm::slerp(orientation_c, orientation_d, blend_factor);
        glm::vec3 blended_parent_scale = glm::mix(scale_d, scale_d, blend_factor);

        glm::mat4 parent_transform = glm::translate(glm::mat4(1.0f), blended_parent_translation) * glm::mat4_cast(blended_parent_orientation) * glm::scale(glm::mat4(1.0f), blended_parent_scale);
        glm::mat4 node_transform = glm::translate(glm::mat4(1.0f), blended_translation) * glm::mat4_cast(blended_orientation) * glm::scale(glm::mat4(1.0f), blended_scale);
        blended_transforms[bone_index] = parent_transform * node_transform * m_Model->m_BoneInfo[bone_index].offset_matrix;
    }

#if 0

    for (auto &state : m_States)
    {
        if (processed_anim_indices.contains(state.anim_index) || total_weight <= 0.0f)
        {
            state_index++;
            continue;
        }

        const f32 normalized_weight = weight_map[state.anim_index];
        SkeletalAnimation &anim = m_Model->GetAnimations()[state.anim_index];



#if 0

        glm::vec3 translation_a(0.0f), scale_a(1.0f);
        glm::quat orientation_a(1.0f, 0.0f, 0.0f, 0.0f);
        glm::mat4 global_transform(1.0f);

        for (const auto &[bone_name, bone_index] : m_Model->m_BoneNameToIndexMap)
        {
            translation_a = state.anim_nodes[bone_name].translation;
            orientation_a = state.anim_nodes[bone_name].rotation;
            scale_a = state.anim_nodes[bone_name].scale;

            glm::vec4 perspective;
            glm::vec3 translation_b, scale_b, skew;
            glm::quat orientation_b;
            glm::decompose(blended_transforms[bone_index], scale_b, orientation_b, translation_b, skew, perspective);

            glm::vec3 blended_translation = glm::mix(translation_a, translation_b, blend_factor);
            glm::quat blended_orientation = glm::slerp(orientation_a, orientation_b, blend_factor);
            glm::vec3 blended_scale = glm::mix(scale_a, scale_b, blend_factor);

            glm::mat4 node_transform = glm::translate(glm::mat4(1.0f), blended_translation) * glm::mat4_cast(blended_orientation) * glm::scale(blended_scale);
            blended_transforms[bone_index] = state.anim_nodes[bone_name].parent_transform * m_Model->m_BoneInfo[bone_index].offset_matrix * node_transform;
        }
#endif

        processed_anim_indices.insert(state.anim_index);
        state_index++;
    }

#endif

    for (size_t i = 0; i < m_Model->m_BoneInfo.size(); ++i)
    {
        m_Model->m_FinalBoneTransforms[i] = blended_transforms[i];
    }
}

f32 AnimationBlender::CalculateWeight(const glm::vec2 &current_pos, const glm::vec2 &position)
{
    // Calculate normalized distances for each axis
    glm::vec2 distance = glm::abs(current_pos - position) / (m_MaxSize - m_MinSize);

    f32 normalized_distance = glm::length(distance);
    f32 max_blend_distance = 1.0f; // Use a normalized distance scale (0 to 1)

    // Calculate weight
    return std::max(0.0f, 1.0f - (normalized_distance / max_blend_distance));
}

}

