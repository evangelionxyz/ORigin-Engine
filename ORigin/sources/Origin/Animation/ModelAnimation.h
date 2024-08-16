// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Animation.h"

#include "Origin/Core/Assert.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <filesystem>

#include "Origin/Math/Math.h"

#include <map>

namespace origin
{
    struct BoneInfo;
    class MeshData;

    template<typename T>
    struct KeyFrame
    {
        T Value;
        float Timestamp;
    };

    using Vec3KeyFrames = std::vector<KeyFrame<glm::vec3>>;
    using QuatKeyFrames = std::vector<KeyFrame<glm::quat>>;

    class Bone
    {
    public:
        Vec3KeyFrames PositionKeyFrames;
        QuatKeyFrames RotationKeyFrames;
        Vec3KeyFrames ScaleKeyFrames;

        std::string Name;
        glm::mat4 LocalTransform;
        int ID;

        Bone(const std::string &name, int id, const aiNodeAnim *channel)
            : Name(name), ID(id), LocalTransform(1.0f)
        {
            for (uint32_t positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex)
            {
                aiVector3D aiPos = channel->mPositionKeys[positionIndex].mValue;
                float timestamp = (float)channel->mPositionKeys[positionIndex].mTime;
                PositionKeyFrames.push_back({ Math::AssimpToGlmVec3(aiPos), timestamp });
            }

            for (uint32_t rotationIndex = 0; rotationIndex < channel->mNumPositionKeys; ++rotationIndex)
            {
                aiQuaternion aiQuat = channel->mRotationKeys[rotationIndex].mValue;
                float timestamp = (float)channel->mRotationKeys[rotationIndex].mTime;
                RotationKeyFrames.push_back({ Math::AssimpToGlmQuat(aiQuat), timestamp });
            }

            for (uint32_t scaleIndex = 0; scaleIndex < channel->mNumScalingKeys; ++scaleIndex)
            {
                aiVector3D aiPos = channel->mScalingKeys[scaleIndex].mValue;
                float timestamp = (float)channel->mScalingKeys[scaleIndex].mTime;
                ScaleKeyFrames.push_back({ Math::AssimpToGlmVec3(aiPos), timestamp });
            }
        }

        void Update(float animTime)
        {
            glm::mat4 translation = InterpolatePosition(animTime);
            glm::mat4 rotation = InterpolateRotation(animTime);
            glm::mat4 scale = InterpolateScaling(animTime);
            LocalTransform = translation * rotation * scale;
        }

        int GetPositionIndex(float animTime)
        {
            for (int i = 0; i < PositionKeyFrames.size() - 1; ++i)
                if (animTime < PositionKeyFrames[i + 1].Timestamp)
                    return i;
            return 0;
        }

        int GetRotationIndex(float animTime)
        {
            for (int i = 0; i < RotationKeyFrames.size() - 1; ++i)
                if (animTime < RotationKeyFrames[i + 1].Timestamp)
                    return i;
            return 0;
        }

        int GetScaleIndex(float animTime)
        {
            for (int i = 0; i < ScaleKeyFrames.size() - 1; ++i)
                if (animTime < ScaleKeyFrames[i + 1].Timestamp)
                    return i;
            return 0;
        }

    private:
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animTime)
        {
            float midWayLength = animTime - lastTimeStamp;
            float framesDiff = nextTimeStamp - lastTimeStamp;
            float scaleFactor = midWayLength / framesDiff;
            return scaleFactor;
        }

        glm::mat4 InterpolatePosition(float animTime)
        {
            if (PositionKeyFrames.size() == 1)
            {
                return glm::translate(glm::mat4(1.0f), PositionKeyFrames[0].Value);
            }

            int p0Index = GetPositionIndex(animTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(PositionKeyFrames[p0Index].Timestamp, PositionKeyFrames[p1Index].Timestamp, animTime);
            glm::vec3 finalPosition = glm::mix(PositionKeyFrames[p0Index].Value, PositionKeyFrames[p1Index].Value, scaleFactor);

            return glm::translate(glm::mat4(1.0f), finalPosition);
        }

        glm::mat4 InterpolateRotation(float animTime)
        {
            if (RotationKeyFrames.size() == 1)
            {
                auto rotation = glm::normalize(RotationKeyFrames[0].Value);
                return glm::toMat4(rotation);
            }

            int p0Index = GetRotationIndex(animTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(RotationKeyFrames[p0Index].Timestamp, RotationKeyFrames[p1Index].Timestamp, animTime);
            glm::quat finalRotation = glm::slerp(RotationKeyFrames[p0Index].Value, RotationKeyFrames[p1Index].Value, scaleFactor);
            finalRotation = glm::normalize(finalRotation);
            return glm::toMat4(finalRotation);
        }

        glm::mat4 InterpolateScaling(float animTime)
        {
            if (ScaleKeyFrames.size() == 1)
            {
                return glm::scale(glm::mat4(1.0f), ScaleKeyFrames[0].Value);
            }

            int p0Index = GetScaleIndex(animTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(ScaleKeyFrames[p0Index].Timestamp, ScaleKeyFrames[p1Index].Timestamp, animTime);
            glm::vec3 finalScale = glm::mix(ScaleKeyFrames[p0Index].Value, ScaleKeyFrames[p1Index].Value, scaleFactor);
            return glm::scale(glm::mat4(1.0f), finalScale);
        }
    };

    struct AssimpNodeData
    {
        glm::mat4 Transformation;
        std::string Name;
        int ChildrendCount;
        std::vector<AssimpNodeData> Children;
    };

    class ModelAnimation : public Animation
    {
    public:
        ModelAnimation() = default;
        ModelAnimation(MeshData *data, aiAnimation *anim, const aiScene *scene);

        void ReadHierarchy(AssimpNodeData &dest, const aiNode *src);
        void ReadMissingBones(MeshData *data, const aiAnimation *anim);

        Bone *FindBone(const std::string &name);

        float GetTicksPersecond() const { return m_TicksPerSecond; }
        float GetDuration() const { return m_Duration; }
        const std::string &GetName() const { return m_Name; }
        const AssimpNodeData &GetRootNode() const { return m_RootNode; }

        static AnimationType GetStaticType() { return AnimationType::Skeletal; }
        AnimationType GetType() const override { return GetStaticType(); }

    private:
        std::string m_Name;
        float m_Duration;
        float m_TicksPerSecond;
        std::vector<Bone> m_Bones;
        std::map<std::string, BoneInfo> m_BoneInfoMap;
        AssimpNodeData m_RootNode;

        friend class Animator;
    };
}