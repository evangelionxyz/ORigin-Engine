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

namespace origin {

struct BoneInfo;
class Mesh;

template<typename T>
struct KeyFrame
{
    T Value;
    f32 Timestamp;
};

template<typename T>
using KeyFrames = std::vector<KeyFrame<T>>;

struct TransformKeyFrameBase
{
protected:
    f32 GetScaleFactor(f32 lastTimeStamp, f32 nextTimeStamp, f32 animTime)
    {
        f32 midWayLength = animTime - lastTimeStamp;
        f32 framesDiff = nextTimeStamp - lastTimeStamp;
        f32 scaleFactor = midWayLength / framesDiff;
        return scaleFactor;
    }
};

struct Vec3Key : TransformKeyFrameBase
{
    KeyFrames<glm::vec3> Frames;

    void AddFrame(const KeyFrame<glm::vec3> &keyFrame)
    {
        Frames.push_back(keyFrame);
    }

    i32 GetIndex(f32 animTime)
    {
        for (i32 i = 0; i < Frames.size() - 1; ++i)
            if (animTime < Frames[i + 1].Timestamp)
                return i;
        return 0;
    }

    glm::mat4 InterpolateTranslation(f32 animTime)
    {
        if (Frames.size() == 1) return glm::translate(glm::mat4(1.0f), Frames[0].Value);
        i32 p0Index = GetIndex(animTime);
        i32 p1Index = p0Index + 1;
        f32 scaleFactor = GetScaleFactor(Frames[p0Index].Timestamp, Frames[p1Index].Timestamp, animTime);
        glm::vec3 finalVector = glm::mix(Frames[p0Index].Value, Frames[p1Index].Value, scaleFactor);
        return glm::translate(glm::mat4(1.0f), finalVector);
    }

    glm::mat4 InterpolateScaling(f32 animTime)
    {
        if (Frames.size() == 1) return glm::scale(glm::mat4(1.0f), Frames[0].Value);
        i32 p0Index = GetIndex(animTime);
        i32 p1Index = p0Index + 1;
        f32 scaleFactor = GetScaleFactor(Frames[p0Index].Timestamp, Frames[p1Index].Timestamp, animTime);
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

    i32 GetIndex(f32 animTime)
    {
        for (i32 i = 0; i < Frames.size() - 1; ++i)
            if (animTime < Frames[i + 1].Timestamp)
                return i;
        return 0;
    }

    glm::mat4 Interpolate(f32 animTime)
    {
        if (Frames.size() == 1)
        {
            auto rotation = glm::normalize(Frames[0].Value);
            return glm::toMat4(rotation);
        }

        i32 p0Index = GetIndex(animTime);
        i32 p1Index = p0Index + 1;
        f32 scaleFactor = GetScaleFactor(Frames[p0Index].Timestamp, Frames[p1Index].Timestamp, animTime);
        glm::quat finalRotation = glm::slerp(Frames[p0Index].Value, Frames[p1Index].Value, scaleFactor);
        finalRotation = glm::normalize(finalRotation);
        return glm::toMat4(finalRotation);
    }
};

class Bone
{
public:
    Bone() = default;
    Bone(const std::string &name, i32 id, const aiNodeAnim *channel);
    void Update(f32 animTime);

    Vec3Key TranslationKeys;
    QuatKey RotationKeys;
    Vec3Key ScaleKeys;

    i32 ID;
    glm::mat4 LocalTransform;
    std::string Name;
};

struct AssimpNodeData
{
    i32 num_children;
    glm::mat4 transform;
    std::string name;
    std::vector<std::string> mesh_names;
    std::vector<AssimpNodeData> children;
};

class ModelAnimation : public Animation
{
public:
    ModelAnimation() = default;
    ModelAnimation(const std::vector<Ref<Mesh>> &meshes, aiAnimation *anim, const aiScene *scene);
    void ReadHierarchy(AssimpNodeData &dest, const aiNode *src, const aiScene *scene);
    void ReadChannels(Ref<Mesh> mesh, const aiAnimation *anim);
    Bone *FindBone(const std::string &name);
    std::unordered_map<std::string, BoneInfo> &GetBoneInfoMap() { return m_AllBoneInfoMap; }
    const AssimpNodeData &GetRootNode() const { return root_node; }
    static AnimationType GetStaticType() { return AnimationType::Skeletal; }
    AnimationType GetType() const override { return GetStaticType(); }
    std::vector<Ref<Mesh>> meshes;
    std::vector<glm::mat4> global_bone_transforms;
    std::string name;
    f32 duration;
    f32 ticks_per_second;
    AssimpNodeData root_node;

private:
    std::unordered_map<std::string, Bone> m_Bones;
    std::unordered_map<std::string, BoneInfo> m_AllBoneInfoMap;
    friend class Animator;
};

}

#endif