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
    Bone::Bone(const std::string &name, int id, const aiNodeAnim *channel)
        : Name(name), ID(id), LocalTransform(1.0f)
    {
        for (u32 positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex)
        {
            aiVector3D aiPos = channel->mPositionKeys[positionIndex].mValue;
            float timestamp = (float)channel->mPositionKeys[positionIndex].mTime;
            TranslationKeys.AddFrame({ Math::AssimpToGlmVec3(aiPos), timestamp });
        }

        for (u32 rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
        {
            aiQuaternion aiQuat = channel->mRotationKeys[rotationIndex].mValue;
            float timestamp = (float)channel->mRotationKeys[rotationIndex].mTime;
            RotationKeys.AddFrame({ Math::AssimpToGlmQuat(aiQuat), timestamp });
        }

        for (u32 scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex)
        {
            aiVector3D aiScale = channel->mScalingKeys[scaleIndex].mValue;
            float timestamp = (float)channel->mScalingKeys[scaleIndex].mTime;
            ScaleKeys.AddFrame({ Math::AssimpToGlmVec3(aiScale), timestamp });
        }
    }

    void Bone::Update(float animTime)
    {
        glm::mat4 translation = TranslationKeys.InterpolateTranslation(animTime);
        glm::mat4 rotation = RotationKeys.Interpolate(animTime);
        glm::mat4 scale = ScaleKeys.InterpolateScaling(animTime);
        LocalTransform = translation * rotation * scale;
    }

    ModelAnimation::ModelAnimation(const std::vector<Ref<MeshData>> &meshes, aiAnimation* anim, const aiScene* scene)
    {
        m_Name = anim->mName.C_Str();

        m_TicksPerSecond = (float)anim->mTicksPerSecond;
        if (anim->mTicksPerSecond == 0.0)
        {
            m_TicksPerSecond = 1.0f;
        }

        m_Duration = (float)anim->mDuration;

        m_Bones.clear();
        ReadHierarchy(m_RootNode, scene->mRootNode);
        
        for (auto &m : meshes)
        {
            ReadMissingBones(m.get(), anim);
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
            if (!boneInfoMap.contains(boneName))
            {
                boneInfoMap[boneName].ID = boneCount;
                boneCount++;
            }

            m_Bones.insert({ boneName, Bone(boneName, boneInfoMap[boneName].ID, channel) });
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