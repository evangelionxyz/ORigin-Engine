// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin/Renderer/UniformBuffer.h"

namespace origin {

static constexpr u32 LIGHTING_BUFFER_SIZE          = sizeof(i32) * 4;
static constexpr u32 DIRECTIONAL_LIGHT_BUFFER_SIZE = sizeof(glm::vec3) * 2;

struct LightingManager
{
    i32 directional_light_count = 0;
    i32 spot_light_count = 0;
    i32 point_light_count = 0;
    i32 area_light_count = 0;
    
    Ref<UniformBuffer> lighting_uniform_buffer;
    
    Ref<UniformBuffer> directional_uniform_buffer;
    Ref<UniformBuffer> spot_uniform_buffer;
    Ref<UniformBuffer> point_uniform_buffer;
    Ref<UniformBuffer> area_uniform_buffer;

    LightingManager();
    
    void Bind() const;
    void Unbind() const;
};

}