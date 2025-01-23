// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "Animator.h"
#include "Origin/Math/Math.h"
#include "Origin/Renderer/Model/Mesh.hpp"

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
        m->bone_transforms.resize(100, glm::mat4(1.0f));
    }
}

void Animator::UpdatePose(const AssimpNodeData *node, const glm::mat4 &parent_transform)
{
    glm::mat4 node_transform = node->transform;
    AnimationNode *bone = current_animation->FindBone(node->name);

    if (bone)
    {
        bone->Update(time_in_ticks);
        node_transform = bone->local_transform;
    }

    glm::mat4 global_transform = parent_transform * node_transform;
    /*if (!current_animation->meshes.empty())
    {
        auto &bone_info_map = current_animation->GetBoneInfoMap();

        glm::mat4 bone_offset = glm::mat4(1.0f);
        if (bone_info_map.contains(node->name))
        {
            const i32 index = bone_info_map[node->name].ID;
            bone_offset = bone_info_map[node->name].offset_matrix;
            if (index < current_animation->global_bone_transforms.size())
            {
                current_animation->global_bone_transforms[index] = global_transform * bone_offset;
            }
        }
    }*/

    for (i32 i = 0; i < node->num_children; i++)
    {
        UpdatePose(&node->children[i], global_transform);
    }
}

}