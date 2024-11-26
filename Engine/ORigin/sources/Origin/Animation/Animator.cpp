// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "Animator.h"
#include "Origin/Math/Math.h"
#include "Origin/Renderer/Mesh.h"

namespace origin
{
    Animator::Animator(ModelAnimation *animation)
        : current_animation(animation), time_in_ticks(0), current_time(0.0f), delta_time(0.0f)
    {
        for (auto &m : current_animation->meshes)
        {
            m->final_bone_matrices.resize(100, glm::mat4(1.0f));
        }
    }

    void Animator::UpdateAnimation(const f32 dt, const f32 speed)
    {
        delta_time = dt;
        if (current_animation)
        {
            current_time += dt * speed;
            time_in_ticks = current_time * current_animation->ticks_per_second;
            time_in_ticks = fmod(time_in_ticks, current_animation->duration);
            CalculateBoneTransform(&current_animation->GetRootNode(), glm::mat4(1.0f));
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

    void Animator::CalculateBoneTransform(const AssimpNodeData *node, const glm::mat4 &parent_transform)
    {
        glm::mat4 nodeTransform = node->Transformation;

        Bone *bone = current_animation->FindBone(node->Name);
        if (bone)
        {
            bone->Update(time_in_ticks);
            nodeTransform = bone->LocalTransform;
        }

        glm::mat4 global_transform = parent_transform * nodeTransform;

        for (auto &mesh: current_animation->meshes)
        {
            auto bone_info_map = mesh->bone_info_map;
            if (bone_info_map.contains(node->Name))
            {
                const i32 index = bone_info_map[node->Name].ID;
                const glm::mat4 &offset = bone_info_map[node->Name].offset_matrix;
                mesh->final_bone_matrices[index] = global_transform * offset;
            }
        }

        for (i32 i = 0; i < node->ChildrenCount; i++)
        {
            CalculateBoneTransform(&node->Children[i], global_transform);
        }
    }
}