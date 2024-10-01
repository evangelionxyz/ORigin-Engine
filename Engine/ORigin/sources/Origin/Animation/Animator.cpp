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

    void Animator::UpdateAnimation(f32 deltaTime, f32 speed)
    {
        m_DeltaTime = deltaTime;
        if (m_CurrentAnimation)
        {
            m_CurrentTime += deltaTime * speed;
            m_TimeInTicks = m_CurrentTime * m_CurrentAnimation->GetTicksPersecond();
            m_TimeInTicks = fmod(m_TimeInTicks, m_CurrentAnimation->m_Duration);
            //const glm::mat4 &parentTransform = CalculateLocalTransform();
            CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
        }
    }

    void Animator::PlayAnimation(ModelAnimation *animation)
    {
        m_FinalBoneMatrices.resize(100, glm::mat4(1.0f));
        m_CurrentAnimation = animation;
        m_CurrentTime = 0.0f;
    }

    glm::mat4 Animator::CalculateLocalTransform()
    {
        glm::mat4 translation = m_CurrentAnimation->m_TranslationKeys.InterpolateTranslation(m_TimeInTicks);
        glm::mat4 rotation = m_CurrentAnimation->m_RotationKeys.Interpolate(m_TimeInTicks);
        glm::mat4 scale = m_CurrentAnimation->m_ScaleKeys.InterpolateScaling(m_TimeInTicks);
        return translation * rotation * scale;
    }

    void Animator::CalculateBoneTransform(const AssimpNodeData *node, const glm::mat4 &parentTransform)
    {
        glm::mat4 nodeTransform = node->Transformation;

        Bone *bone = m_CurrentAnimation->FindBone(node->Name);
        if (bone)
        {
            bone->Update(m_TimeInTicks);
            nodeTransform = bone->LocalTransform;
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform;

        std::map<std::string, BoneInfo> &boneInfoMap = m_CurrentAnimation->m_BoneInfoMap;
        if (boneInfoMap.contains(node->Name))
        {
            i32 index = boneInfoMap[node->Name].ID;
            m_FinalBoneMatrices[index] = globalTransform * boneInfoMap[node->Name].OffsetMatrix;
        }

        for (i32 i = 0; i < node->ChildrenCount; i++)
        {
            CalculateBoneTransform(&node->Children[i], globalTransform);
        }
    }

    std::vector<glm::mat4> Animator::GetFinalBoneMatrices()
    {
        return m_FinalBoneMatrices;
    }
}