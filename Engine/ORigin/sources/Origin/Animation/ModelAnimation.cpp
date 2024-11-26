// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ModelAnimation.h"
#include "Origin/Math/Math.h"
#include "Origin/Renderer/Mesh.h"

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

    ModelAnimation::ModelAnimation(std::vector<Ref<Mesh>> meshes, aiAnimation *anim, const aiScene *scene)
        : meshes(meshes)
    {
        name = anim->mName.C_Str();

        ticks_per_second = (float)anim->mTicksPerSecond;
        if (anim->mTicksPerSecond == 0.0)
        {
            ticks_per_second = 1.0f;
        }

        duration = static_cast<float>(anim->mDuration);

        m_Bones.clear();
        ReadHierarchy(root_node, scene->mRootNode);
        
        for (auto &mesh : meshes)
        {
            ReadMissingBones(mesh, anim);
        }
    }

    void ModelAnimation::ReadMissingBones(Ref<Mesh> mesh, const aiAnimation *anim)
    {
        auto &bone_info_map = mesh->bone_info_map;
        int &bone_count = mesh->bone_count;

        for (u32 i = 0; i < anim->mNumChannels; ++i)
        {
            // find animation bone data
            aiNodeAnim *channel = anim->mChannels[i];

            const std::string &bone_name = channel->mNodeName.data;

            // if it is not found in mesh data, then add it
            if (!bone_info_map.contains(bone_name))
            {
                bone_info_map[bone_name].ID = bone_count;
                bone_count++;
            }

            m_Bones.insert({ bone_name, Bone(bone_name, bone_info_map[bone_name].ID, channel) });
        }
    }

    void ModelAnimation::ReadHierarchy(AssimpNodeData &dest, const aiNode *src)
    {
        dest.Name = src->mName.C_Str();
        dest.Transformation = Math::AssimpToGlmMatrix(src->mTransformation);

        dest.ChildrenCount = src->mNumChildren;

        for (u32 i = 0; i < src->mNumChildren; ++i)
        {
            AssimpNodeData new_data;
            ReadHierarchy(new_data, src->mChildren[i]);
            dest.Children.push_back(new_data);
        }
    }

    Bone *ModelAnimation::FindBone(const std::string &name)
    {
        if (m_Bones.contains(name))
        {
            return &m_Bones.at(name);
        }

        return nullptr;
    }
}