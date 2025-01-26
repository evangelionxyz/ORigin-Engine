// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include <Origin/Animation/Animation.h>

#include "PanelBase.hpp"

namespace origin {

class BlendSpacePanel : public PanelBase
{
public:
    struct BlendSpaceControl
    {
        Rect rect;
        glm::vec2 current_position;
    };
    
    BlendSpacePanel();
    ~BlendSpacePanel() override;

    void AddAnimation(SkeletalAnimation* animation);

    void Render() override;
    void OnUpdate(f32 delta_time) override;
    void OnEvent(Event &e) override;

private:
    std::vector<SkeletalAnimation *> m_animations;
};

}
