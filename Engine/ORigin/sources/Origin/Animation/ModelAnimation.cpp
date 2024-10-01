// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ModelAnimation.h"
#include "Origin/Math/Math.h"
#include "Origin/Renderer/MeshVertexData.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace origin
{
    ModelAnimation::ModelAnimation(MeshData *data, aiAnimation *anim, const aiScene *scene)
    {
        m_Name = anim->mName.C_Str();

        m_TicksPerSecond = (float)anim->mTicksPerSecond;
        if (anim->mTicksPerSecond == 0.0)
            m_TicksPerSecond = 1.0f;

        m_Duration = (float)anim->mDuration;

        m_Bones.clear();
        ReadHierarchy(m_RootNode, scene->mRootNode);
        
        aiNodeAnim *rootChannel = anim->mChannels[0];
        ReadLocalTransform(rootChannel);

        // find bones if it is not found in data at first load
        if (data->boneCount)
        {
            ReadMissingBones(data, anim);
        }
            
    }

    void ModelAnimation::ReadMissingBones(MeshData *data, const aiAnimation *anim)
    {
        auto &boneInfoMap = data->boneInfoMap;
        int &boneCount = data->boneCount;

        for (u32 i = 0; i < anim->mNumChannels; ++i)
        {
            // find animation bone data
            aiNodeAnim *channel = anim->mChannels[i];

            const std::string &boneName = channel->mNodeName.data;

            // if it is not found in mesh data, then add it
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].ID = boneCount;
                boneCount++;
            }

            m_Bones.insert({ boneName, Bone(boneName, boneInfoMap[boneName].ID, channel) });
        }

        // set current bone info map
        // to access in Animator
        m_BoneInfoMap = boneInfoMap;
    }

    void ModelAnimation::ReadLocalTransform(aiNodeAnim *channel)
    {
        for (u32 positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex)
        {
            aiVector3D aiPos = channel->mPositionKeys[positionIndex].mValue;
            float timestamp = (float)channel->mPositionKeys[positionIndex].mTime;
            m_TranslationKeys.AddFrame({ Math::AssimpToGlmVec3(aiPos), timestamp });
        }

        for (u32 rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
        {
            aiQuaternion aiQuat = channel->mRotationKeys[rotationIndex].mValue;
            float timestamp = (float)channel->mRotationKeys[rotationIndex].mTime;
            m_RotationKeys.AddFrame({ Math::AssimpToGlmQuat(aiQuat), timestamp });
        }

        for (u32 scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex)
        {
            aiVector3D aiScale = channel->mScalingKeys[scaleIndex].mValue;
            float timestamp = (float)channel->mScalingKeys[scaleIndex].mTime;
            m_ScaleKeys.AddFrame({ Math::AssimpToGlmVec3(aiScale), timestamp });
        }
    }

    void ModelAnimation::ReadHierarchy(AssimpNodeData &dest, const aiNode *src)
    {
        dest.Name = src->mName.C_Str();
        dest.Transformation = Math::AssimpToGlmMatrix(src->mTransformation);

        dest.ChildrenCount = src->mNumChildren;

        for (u32 i = 0; i < src->mNumChildren; ++i)
        {
            AssimpNodeData newData;
            ReadHierarchy(newData, src->mChildren[i]);
            dest.Children.push_back(newData);
        }
    }
    Bone *ModelAnimation::FindBone(const std::string &name)
    {
        if (m_Bones.contains(name))
            return &m_Bones.at(name);

        return nullptr;
    }
}