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

void AnimationBlender::AddAnimation(i32 anim_index, const glm::vec2 &blend_position)
{
    m_AnimationStates.push_back({ anim_index, 0.0f, blend_position });
}

void AnimationBlender::UpdateBlending(const glm::vec2 &current_position)
{
    for (auto &state : m_AnimationStates)
        state.weight = CalculateWeight(current_position, state.blend_position);
}

void AnimationBlender::BlendAnimations(f32 delta_time)
{
    m_Model->m_FinalBoneTransforms.resize(m_Model->m_BoneInfo.size());
    std::vector<glm::mat4> blended_transforms(m_Model->m_BoneInfo.size(), glm::mat4(1.0f));

    std::vector<glm::vec3> blend_translations(m_Model->m_BoneInfo.size(), glm::vec3(0.0f));
    std::vector<glm::vec3> blend_scales(m_Model->m_BoneInfo.size(), glm::vec3(0.0f));
    std::vector<glm::quat> blend_rotations(m_Model->m_BoneInfo.size(), glm::quat(1.0f, 0.0f, 0.0f, 0.0f));

    // prepare weight tracking
    f32 total_weight = 0.0f;
    std::vector<f32> weights;
    for (const auto &state : m_AnimationStates)
    {
        f32 weight = state.weight;
        weights.push_back(weight);
        total_weight += weight;
    }

    // normalize weights
    if (total_weight >= 0.0f)
    {
        for (auto &weight : weights)
            weight /= total_weight;
    }

    // blend transforms
    for (size_t anim_idx = 0; anim_idx < m_AnimationStates.size(); ++anim_idx)
    {
        auto &state = m_AnimationStates[anim_idx];

        if (state.weight > 0.0f)
        {
            SkeletalAnimation &anim = m_Model->GetAnimations()[state.anim_index];
            anim.UpdateTime(delta_time);
            glm::mat4 identity(1.0f);

            m_Model->CalculateBoneTransforms(anim.GetTimeInTicks(),
                m_Model->m_Scene->mRootNode,
                identity,
                state.anim_index);

            // Decompose and blend transforms
            for (size_t i = 0; i < m_Model->m_BoneInfo.size(); ++i)
            {
                glm::vec3 translation, scale;
                glm::quat rotation;
                glm::vec3 skew;
                glm::vec4 perspective;

                glm::decompose(
                    m_Model->m_BoneInfo[i].final_transformation,
                    scale,
                    rotation,
                    translation,
                    skew,
                    perspective
                );

                blend_translations[i] += translation * weights[anim_idx];
                blend_rotations[i] = glm::slerp(blend_rotations[i], rotation, weights[anim_idx]);
                blend_scales[i] += scale * weights[anim_idx];
            }
        }
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

f32 AnimationBlender::CalculateWeight(const glm::vec2 &current_pos, const glm::vec2 &anim_pos)
{
    f32 distance = glm::length(current_pos - anim_pos);
    f32 max_blend_distance = 2.0f;
    return std::max(0.0f, 1.0f - (distance / max_blend_distance));
}

}

