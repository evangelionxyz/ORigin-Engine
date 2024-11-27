// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "Animator.h"
#include "Origin/Math/Math.h"
#include "Origin/Renderer/Mesh.h"

namespace origin {
Animator::Animator(ModelAnimation *animation)
    : current_animation(animation), time_in_ticks(0), current_time(0.0f), delta_time(0.0f)
{
}

void Animator::UpdateAnimation(const f32 dt, const f32 speed)
{
    delta_time = dt;
    if (current_animation)
    {
        current_time += dt * speed;
        time_in_ticks = current_time * current_animation->ticks_per_second;
        time_in_ticks = fmod(time_in_ticks, current_animation->duration);

        std::fill(current_animation->global_bone_transforms.begin(),
            current_animation->global_bone_transforms.end(),
            glm::mat4(1.0f));

        UpdatePose(&current_animation->GetRootNode(), glm::mat4(1.0f));
    }
}

void Animator::PlayAnimation(ModelAnimation *animation)
{
    current_animation = animation;
    current_time = 0.0f;

    for (auto &m : current_animation->meshes)
    {
        m->final_bone_matrices.resize(100, glm::mat4(1.0f));
    }
}

void Animator::UpdatePose(const AssimpNodeData *node, const glm::mat4 &parent_transform)
{
    glm::mat4 node_transform = node->Transformation;
    Bone *bone = current_animation->FindBone(node->Name);

    if (bone)
    {
        bone->Update(time_in_ticks);
        node_transform = bone->LocalTransform;
    }

    glm::mat4 global_transform = parent_transform * node_transform;

    for (auto &name : node->mesh_names)
    {
        for (auto &mesh : current_animation->meshes)
        {
            if (mesh->name == name) mesh->transform = global_transform;
        }
    }

    if (!current_animation->meshes.empty())
    {
        auto &bone_info_map = current_animation->meshes[0]->bone_info_map;
        if (bone_info_map.contains(node->Name))
        {
            const i32 index = bone_info_map[node->Name].ID;
            const glm::mat4 &offset = bone_info_map[node->Name].offset_matrix;
            if (index < current_animation->global_bone_transforms.size())
            {
                current_animation->global_bone_transforms[index] = global_transform * offset;
            }
        }
    }

    for (i32 i = 0; i < node->ChildrenCount; i++)
        UpdatePose(&node->Children[i], global_transform);
}

void Animator::ApplyToMeshes()
{
    if (!current_animation) return;
    for (auto &mesh : current_animation->meshes)
    {
        for (auto &[bone_name, bone_info] : mesh->bone_info_map)
        {
            if (bone_info.ID < current_animation->global_bone_transforms.size())
            {
                mesh->final_bone_matrices[bone_info.ID] 
                    = current_animation->global_bone_transforms[bone_info.ID];
            }
        }
    }
}

}