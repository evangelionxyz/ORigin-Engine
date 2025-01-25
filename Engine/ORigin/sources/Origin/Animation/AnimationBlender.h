// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Core/Types.h"
#include "Origin/Math/Math.h"

#include <glm/glm.hpp>
#include <map>

namespace origin {

class Model;

class AnimationBlender
{
public:
    struct BlendState
    {
        i32 anim_index = 0;
        f32 weight = 0.0f;
        glm::vec2 position{ 0.0f, 0.0f };
    };

    void SetModel(Ref<Model> &model);
    void SetRange(const glm::vec2 &min_size, const glm::vec2 &max_size);
    void AddAnimation(i32 anim_index, const glm::vec2 &position);
    void BlendAnimations(const glm::vec2 &current_position, f32 delta_time);
    const glm::vec2 GetMaxSize() const { return m_MaxSize; }
    const glm::vec2 GetMinSize() const { return m_MinSize; }

    std::vector<BlendState> GetStates() { return m_States; }

private:
    f32 CalculateWeight(const glm::vec2 &current_pos, const glm::vec2 &position);

    std::vector<BlendState> m_States;

    glm::vec2 m_MaxSize { 10.0f, 10.0f };
    glm::vec2 m_MinSize{ -10.0f, -10.0f };
    Ref<Model> m_Model;
};

}

