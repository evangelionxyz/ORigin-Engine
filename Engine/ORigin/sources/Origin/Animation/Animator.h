// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "ModelAnimation.h"

namespace origin
{
    class Animator
    {
    public:
        Animator() = default;
        Animator(ModelAnimation *animation);

        void UpdateAnimation(f32 deltaTime, f32 speed);
        void PlayAnimation(ModelAnimation *animation);
        void CalculateBoneTransform(const AssimpNodeData *node, const glm::mat4 &parentTransform);

        bool HasAnimation() { return m_CurrentAnimation != nullptr; }

        std::vector<glm::mat4> GetFinalBoneMatrices();

        std::vector<glm::mat4> m_FinalBoneMatrices;
        ModelAnimation *m_CurrentAnimation = nullptr;
        f32 m_TimeInTicks;
        f32 m_CurrentTime;
        f32 m_DeltaTime;
    };
}

#endif