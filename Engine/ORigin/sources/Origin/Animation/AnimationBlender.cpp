// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "AnimationBlender.h"
#include "Animation.h"
#include "Origin/Renderer/Model/Model.hpp"

namespace origin {

void AnimationBlender::SetModel(Ref<Model> &model)
{
    m_Model = model;
}

void AnimationBlender::SetMaxSize(const glm::vec2 &max_size)
{
    m_MaxSize = max_size;
}

void AnimationBlender::AddAnimation(i32 anim_index, const glm::vec2 &position)
{
    m_States.push_back({ anim_index, 0.0f, position });
}

void AnimationBlender::BlendAnimations(const glm::vec2 &current_position, f32 delta_time)
{
    m_Model->m_FinalBoneTransforms.resize(m_Model->m_BoneInfo.size());

    std::vector<glm::mat4> blended_transforms(m_Model->m_BoneInfo.size(), glm::mat4(1.0f));
    std::vector<glm::vec3> blend_translations(m_Model->m_BoneInfo.size(), glm::vec3(0.0f));
    std::vector<glm::vec3> blend_scales(m_Model->m_BoneInfo.size(), glm::vec3(0.0f));
    std::vector<glm::quat> blend_rotations(m_Model->m_BoneInfo.size(), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    for (auto &state : m_States)
    {
        state.weight = CalculateWeight(current_position, state.position);
    }

    f32 total_weight = 0.0f;
    std::vector<f32> normalized_weights;
    std::unordered_map<i32, f32> weight_map;

    for (const auto &state : m_States)
    {
        f32 weight = state.weight;
        normalized_weights.push_back(weight);
        total_weight += weight;

        // accumulate weight per animation index
        weight_map[state.anim_index] += weight;
    }

    // normalize weights
    if (total_weight > 0.0f)
    {
        for (auto &pair : weight_map)
        {
            pair.second /= total_weight;
        }
    }

    // Blend animations
    std::unordered_set<i32> processed_anim_indices; // save processed animation indices for multiple animation position
    i32 state_index = 0;

    for (const auto &state : m_States)
    {
        if (processed_anim_indices.contains(state.anim_index) || state.weight <= 0.0f)
        {
            state_index++;
            continue;
        }

        SkeletalAnimation &anim = m_Model->GetAnimations()[state.anim_index];
        anim.UpdateTime(delta_time);

        glm::mat4 identity(1.0f);
        m_Model->CalculateBoneTransforms(
            anim.GetTimeInTicks(),
            m_Model->m_Scene->mRootNode,
            identity,
            state.anim_index
        );

        // Use the cumulative normalized weight for blending
        const f32 normalized_weight = weight_map[state.anim_index];

        for (size_t i = 0; i < m_Model->m_BoneInfo.size(); ++i)
        {
            glm::vec3 translation, scale;
            glm::quat rotation;
            glm::vec3 skew;
            glm::vec4 perspective;

            glm::decompose(
                m_Model->m_BoneInfo[i].transform,
                scale,
                rotation,
                translation,
                skew,
                perspective
            );

            blend_translations[i] += translation * normalized_weight;
            blend_rotations[i] = glm::slerp(blend_rotations[i], rotation, normalized_weight);
            blend_scales[i] += scale * normalized_weight;
        }

        processed_anim_indices.insert(state.anim_index);
        state_index++;
    }

    // recompose transforms
    for (size_t i = 0; i < m_Model->m_BoneInfo.size(); ++i)
    {
        glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), blend_translations[i]);
        glm::mat4 rotation_matrix = glm::mat4_cast(glm::normalize(blend_rotations[i]));
        glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), blend_scales[i]);
        m_Model->m_FinalBoneTransforms[i] = translation_matrix * rotation_matrix * scale_matrix;
    }
}

f32 AnimationBlender::CalculateWeight(const glm::vec2 &current_pos, const glm::vec2 &position)
{
    f32 distance = glm::length(current_pos - position);
    f32 max_blend_distance = 2.0f;
    return std::max(0.0f, 1.0f - (distance / max_blend_distance));
}

}

