// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef MODEL_ANIMATION_H
#define MODEL_ANIMATION_H

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

    template<typename T>
    using KeyFrames = std::vector<KeyFrame<T>>;

    struct TransformKeyFrameBase
    {
    protected:
        float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animTime)
        {
            float midWayLength = animTime - lastTimeStamp;
            float framesDiff = nextTimeStamp - lastTimeStamp;
            float scaleFactor = midWayLength / framesDiff;
            return scaleFactor;
        }
    };

    struct Vec3Key : public TransformKeyFrameBase
    {
        KeyFrames<glm::vec3> Frames;

        void AddFrame(const KeyFrame<glm::vec3> &keyFrame)
        {
            Frames.push_back(keyFrame);
        }

        int GetIndex(float animTime)
        {
            for (int i = 0; i < Frames.size() - 1; ++i)
                if (animTime < Frames[i + 1].Timestamp)
                    return i;
            return 0;
        }

        glm::mat4 InterpolateTranslation(float animTime)
        {
            if (Frames.size() == 1) return glm::translate(glm::mat4(1.0f), Frames[0].Value);
            int p0Index = GetIndex(animTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(Frames[p0Index].Timestamp, Frames[p1Index].Timestamp, animTime);
            glm::vec3 finalVector = glm::mix(Frames[p0Index].Value, Frames[p1Index].Value, scaleFactor);
            return glm::translate(glm::mat4(1.0f), finalVector);
        }

        glm::mat4 InterpolateScaling(float animTime)
        {
            if (Frames.size() == 1) return glm::scale(glm::mat4(1.0f), Frames[0].Value);
            int p0Index = GetIndex(animTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(Frames[p0Index].Timestamp, Frames[p1Index].Timestamp, animTime);
            glm::vec3 finalScale = glm::mix(Frames[p0Index].Value, Frames[p1Index].Value, scaleFactor);
            return glm::scale(glm::mat4(1.0f), finalScale);
        }
    };

    struct QuatKey : public TransformKeyFrameBase
    {
        KeyFrames<glm::quat> Frames;

        void AddFrame(const KeyFrame<glm::quat> &keyFrame)
        {
            Frames.push_back(keyFrame);
        }

        int GetIndex(float animTime)
        {
            for (int i = 0; i < Frames.size() - 1; ++i)
                if (animTime < Frames[i + 1].Timestamp)
                    return i;
            return 0;
        }

        glm::mat4 Interpolate(float animTime)
        {
            if (Frames.size() == 1)
            {
                auto rotation = glm::normalize(Frames[0].Value);
                return glm::toMat4(rotation);
            }

            int p0Index = GetIndex(animTime);
            int p1Index = p0Index + 1;
            float scaleFactor = GetScaleFactor(Frames[p0Index].Timestamp, Frames[p1Index].Timestamp, animTime);
            glm::quat finalRotation = glm::slerp(Frames[p0Index].Value, Frames[p1Index].Value, scaleFactor);
            finalRotation = glm::normalize(finalRotation);
            return glm::toMat4(finalRotation);
        }
    };

    class Bone
    {
    public:
        Vec3Key TranslationKeys;
        QuatKey RotationKeys;
        Vec3Key ScaleKeys;

        std::string Name;
        glm::mat4 LocalTransform;
        int ID;

        Bone() = default;
        Bone(const std::string &name, int id, const aiNodeAnim *channel);

        void Update(float animTime);
    };

    struct AssimpNodeData
    {
        glm::mat4 Transformation;
        std::string Name;
        int ChildrenCount;
        std::vector<AssimpNodeData> Children;
    };

    class ModelAnimation : public Animation
    {
    public:
        ModelAnimation() = default;
        ModelAnimation(const std::vector<Ref<MeshData>> &meshes, aiAnimation *anim, const aiScene *scene);

        void ReadHierarchy(AssimpNodeData &dest, const aiNode *src);
        void ReadMissingBones(MeshData *mesh, const aiAnimation *anim);

        Bone *FindBone(const std::string &name);

        float GetTicksPersecond() const { return m_TicksPerSecond; }
        float GetDuration() const { return m_Duration; }
        const std::string &GetName() const { return m_Name; }
        const AssimpNodeData &GetRootNode() const { return m_RootNode; }
        const std::unordered_map<std::string, BoneInfo> &GetBoneInfo() { return m_BoneInfo; }

        static AnimationType GetStaticType() { return AnimationType::Skeletal; }
        AnimationType GetType() const override { return GetStaticType(); }

    private:
        std::string m_Name;
        float m_Duration;
        float m_TicksPerSecond;

        std::unordered_map<std::string, Bone> m_Bones;
        std::unordered_map<std::string, BoneInfo> m_BoneInfo;

        AssimpNodeData m_RootNode;
        friend class Animator;
    };
}

#endif