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
    m_States.push_back({ anim_index, 0.0f, position });
}

void AnimationBlender::BlendAnimations(const glm::vec2 &current_position, f32 delta_time, const f32 speed)
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
        if (processed_anim_indices.contains(state.anim_index) || total_weight <= 0.0f)
        {
            state_index++;
            continue;
        }

        SkeletalAnimation &anim = m_Model->GetAnimations()[state.anim_index];
        anim.UpdateTime(delta_time, speed);

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
            glm::vec3 translationA, scaleA, translationB, scaleB, skew;
            glm::quat rotationA, rotationB;
            glm::vec4 perspective;

            // Decompose current blended transform
            glm::decompose(blended_transforms[i], scaleA, rotationA, translationA, skew, perspective);

            // Decompose current animation transform
            glm::decompose(m_Model->m_BoneInfo[i].transform, scaleB, rotationB, translationB, skew, perspective);

            // Interpolate each component
            glm::vec3 blended_translation = glm::mix(translationA, translationB, normalized_weight);
            glm::quat blended_rotation = glm::slerp(rotationA, rotationB, normalized_weight);
            glm::vec3 blended_scale = glm::mix(scaleA, scaleB, normalized_weight);

            // Recompose the transform
            blended_transforms[i] = glm::translate(glm::mat4(1.0f), blended_translation) *
                glm::mat4_cast(blended_rotation) *
                glm::scale(glm::mat4(1.0f), blended_scale);
        }

        processed_anim_indices.insert(state.anim_index);
        state_index++;
    }

    // recompose transforms
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

