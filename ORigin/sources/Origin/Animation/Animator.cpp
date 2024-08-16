// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Animator.h"

#include "Origin/Math/Math.h"

#include "Origin/Renderer/MeshVertexData.h"

namespace origin
{
    Animator::Animator(ModelAnimation *animation)
        : m_CurrentAnimation(animation), m_CurrentTime(0.0f), m_DeltaTime(0.0f)
    {
        m_FinalBoneMatrices.resize(100, glm::mat4(1.0f));
    }

    void Animator::UpdateAnimation(float deltaTime, float speed)
    {
        m_DeltaTime = deltaTime;
        if (m_CurrentAnimation)
        {
            m_CurrentTime += deltaTime * speed;
            m_TimeInTicks = m_CurrentTime * m_CurrentAnimation->GetTicksPersecond();
            m_TimeInTicks = fmod(m_TimeInTicks, m_CurrentAnimation->m_Duration);
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void Animator::PlayAnimation(ModelAnimation *animation)
    {
        m_FinalBoneMatrices.resize(100, glm::mat4(1.0f));
        m_CurrentAnimation = animation;
        m_CurrentTime = 0.0f;
    }

    void Animator::CalculateBoneTransform(const AssimpNodeData *node, glm::mat4 parentTransform)
    {
        std::string nodeName = node->Name;
        glm::mat4 nodeTransform = node->Transformation;

        Bone *bone = m_CurrentAnimation->FindBone(nodeName);
        if (bone)
        {
            bone->Update(m_TimeInTicks);
            nodeTransform = bone->LocalTransform;
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform;

        auto boneInfoMap = m_CurrentAnimation->m_BoneInfoMap;
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].ID;
            glm::mat4 offset = boneInfoMap[nodeName].OffsetMatrix;
            m_FinalBoneMatrices[index] = globalTransform * offset;
        }

        for (int i = 0; i < node->ChildrendCount; i++)
        {
            CalculateBoneTransform(&node->Children[i], globalTransform);
        }
    }

    std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }
}