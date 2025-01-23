// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ModelAnimation.hpp"
#include "Origin/Math/Math.h"
#include "Origin/Renderer/Model/Mesh.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace origin {

ModelAnimation::ModelAnimation(const std::vector<Ref<Mesh>> &meshes, aiAnimation *anim, const aiScene *scene)
{
    name = anim->mName.C_Str();

    ticks_per_second = static_cast<float>(anim->mTicksPerSecond);
    if (anim->mTicksPerSecond == 0.0)
    {
        ticks_per_second = 1.0f;
    }

    duration = static_cast<float>(anim->mDuration);
    global_bone_transforms.resize(100, glm::mat4(1.0f));

    ReadHierarchy(root_node, scene->mRootNode, scene);
    for (auto &mesh : meshes)
    {
        ReadChannels(mesh, anim);
    }
}

void ModelAnimation::ReadChannels(Ref<Mesh> mesh, const aiAnimation *anim)
{
    /*auto &bone_info_map = mesh->bones;

    if (mesh->bones.empty())
        return;

    for (u32 i = 0; i < anim->mNumChannels; ++i)
    {
        aiNodeAnim *node_anim = anim->mChannels[i];
        std::string bone_name(node_anim->mNodeName.C_Str());

        if (!bone_info_map.contains(bone_name))
        {
            bone_info_map[bone_name].id;
        }

        if (!m_Bones.contains(bone_name))
        {
            m_Bones[bone_name] = Bone(bone_name, bone_info_map[bone_name].id, node_anim);
        }
    }*/
}

void ModelAnimation::ReadHierarchy(AssimpNodeData &dest, const aiNode *src, const aiScene *scene)
{
    dest.name = src->mName.C_Str();
    dest.transform = Math::AssimpToGlmMatrix(src->mTransformation);

    dest.num_children = src->mNumChildren;
    for (u32 i = 0; i < src->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[src->mMeshes[i]];
        if (mesh)
        {
            dest.mesh_names.push_back(mesh->mName.data);
        }
    }

    for (u32 i = 0; i < src->mNumChildren; ++i)
    {
        AssimpNodeData new_data;
        ReadHierarchy(new_data, src->mChildren[i], scene);
        dest.children.push_back(new_data);
    }
}

AnimationNode *ModelAnimation::FindBone(const std::string &name)
{
    if (m_Bones.contains(name))
    {
        return &m_Bones.at(name);
    }
    return nullptr;
}

}