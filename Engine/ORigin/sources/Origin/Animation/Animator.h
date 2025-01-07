// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "ModelAnimation.h"

namespace origin
{
    class Animator
    {
    public:
        Animator() = default;
        Animator(ModelAnimation *animation);

        void UpdateAnimation(f32 dt, f32 speed);
        void PlayAnimation(ModelAnimation *animation);
        void UpdatePose(const AssimpNodeData *node,
            const glm::mat4 &parent_transform);
        void ApplyToMeshes();

        bool HasAnimation() { return current_animation != nullptr; }
        
        ModelAnimation *current_animation = nullptr;
        f32 time_in_ticks;
        f32 current_time;
        f32 delta_time;
    };
}

#endif