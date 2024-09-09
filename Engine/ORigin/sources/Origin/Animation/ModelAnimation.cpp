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

        ReadHierarchy(m_RootNode, scene->mRootNode);

        // find bones if it is not found in data at first load
        ReadMissingBones(data, anim);
    }

    void ModelAnimation::ReadMissingBones(MeshData *data, const aiAnimation *anim)
    {
        auto &boneInfoMap = data->boneInfoMap;
        int &boneCount = data->boneCount;

        for (uint32_t i = 0; i < anim->mNumChannels; ++i)
        {
            // find animation bone data
            auto channel = anim->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            // if it is not found in mesh data, then add it
            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].ID = boneCount;
                boneCount++;
            }

            m_Bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].ID, channel));
        }

        // set current bone info map
        // to access in Animator
        m_BoneInfoMap = boneInfoMap;
    }

    void ModelAnimation::ReadHierarchy(AssimpNodeData &dest, const aiNode *src)
    {
        dest.Name = src->mName.data;
        dest.Transformation = Math::AssimpToGlmMatrix(src->mTransformation);

        dest.ChildrendCount = src->mNumChildren;

        for (uint32_t i = 0; i < src->mNumChildren; ++i)
        {
            AssimpNodeData newData;
            ReadHierarchy(newData, src->mChildren[i]);
            dest.Children.push_back(newData);
        }
    }
    Bone *ModelAnimation::FindBone(const std::string &name)
    {
        std::vector<Bone>::iterator it = std::find_if(m_Bones.begin(), m_Bones.end(),
        [&](const Bone &bone)
        {
            return bone.Name == name;
        });

        if (it == m_Bones.end())
        {
            return nullptr;
        }

        return &(*it);
    }
}