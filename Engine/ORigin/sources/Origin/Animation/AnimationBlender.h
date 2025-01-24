// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Core/Types.h"
#include <glm/glm.hpp>

namespace origin {

class Model;

class AnimationBlender
{
public:
    struct BlendState
    {
        i32 anim_index = 0;
        f32 weight = 0.0f;
        glm::vec2 blend_position = { 0.0f, 0.0f };
    };

    void SetModel(Ref<Model> &model);
    void SetMaxSize(const glm::vec2 &max_size);
    void AddAnimation(i32 anim_index, const glm::vec2 &blend_position);
    void UpdateBlending(const glm::vec2 &current_position);
    void BlendAnimations(f32 delta_time);
    glm::vec2 &GetMaxSize() { return m_MaxSize; }
    std::vector<BlendState> GetStates() { return m_AnimationStates; }

private:
    f32 CalculateWeight(const glm::vec2 &current_pos, const glm::vec2 &anim_pos);

    std::vector<BlendState> m_AnimationStates;
    glm::vec2 m_MaxSize {2.0f, 2.0f};
    Ref<Model> m_Model;
};

}

