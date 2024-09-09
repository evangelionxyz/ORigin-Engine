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

        void UpdateAnimation(float deltaTime, float speed);
        void PlayAnimation(ModelAnimation *animation);
        void CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform);

        std::vector<glm::mat4> GetFinalBoneMatrices();

        std::vector<glm::mat4> m_FinalBoneMatrices;
        ModelAnimation *m_CurrentAnimation;
        float m_TimeInTicks;
        float m_CurrentTime;
        float m_DeltaTime;
    };
}

#endif