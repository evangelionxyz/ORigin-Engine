#pragma once

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