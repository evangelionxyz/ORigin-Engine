// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Core/Types.h"
#include "Origin/Math/Math.h"
#include "Animation.h"

#include <glm/glm.hpp>
#include <unordered_map>
#include <string>
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
        std::unordered_map<std::string, AnimationNode> anim_nodes;
    };

    void SetModel(Ref<Model> &model);
    void SetRange(const glm::vec2 &min_size, const glm::vec2 &max_size);
    void AddAnimation(i32 anim_index, const glm::vec2 &position);
    void BlendAnimations(const glm::vec2 &current_position, f32 delta_time, const f32 speed, f32 blend_factor);
    const glm::vec2 GetMaxSize() const { return m_MaxSize; }
    const glm::vec2 GetMinSize() const { return m_MinSize; }

    std::vector<BlendState> GetStates() { return m_States; }

private:
    f32 CalculateWeight(const glm::vec2 &current_pos, const glm::vec2 &position);

    std::vector<BlendState> m_States;

    glm::vec2 m_MaxSize { 10.0f, 10.0f };
    glm::vec2 m_MinSize{ -10.0f, -10.0f };
    std::vector<aiNode *> m_AiNodes;
    Ref<Model> m_Model;
};

}

