// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef MODEL_ANIMATION_HPP
#define MODEL_ANIMATION_HPP

#include "Animation.h"
#include "Origin/Core/Assert.h"
#include "Origin/Math/Math.h"
#include "Origin/Renderer/Model/Bone.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <filesystem>


#include <map>

namespace origin {

struct BoneInfo;
class Mesh;

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
    AnimationNode *FindBone(const std::string &name);
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
    std::unordered_map<std::string, AnimationNode> m_Bones;
    friend class Animator;
};

}

#endif