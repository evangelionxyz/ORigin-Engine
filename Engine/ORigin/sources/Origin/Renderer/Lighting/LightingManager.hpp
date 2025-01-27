// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Lighting.hpp"
#include "Origin/Renderer/UniformBuffer.h"

namespace origin {

struct LightingManager
{
    /// =========================
    ///        SPOT LIGHT
    /// =========================
    size_t intial_spotlight_capacity = 10;
    size_t spotlight_count = 0;
    size_t spotlight_buffer_size = intial_spotlight_capacity * sizeof(SpotLight::Data);
    
    Ref<UniformBuffer> lighting_uniform_buffer;
    Ref<UniformBuffer> directional_uniform_buffer;

    LightingManager();
    
    void Bind() const;
    void Unbind() const;

    void CreateStorageBuffer();
    void AddSpotLight();

    static LightingManager *GetInstance();

private:
    struct Data
    {
        i32 directional_light_count = 0;
        i32 spot_light_count = 0;
        i32 point_light_count = 0;
        i32 area_light_count = 0;
    };
    
    void CreateSpotLightStorageBuffer();

    u32 m_spotlight_SSBO;
    Data m_data;

    friend struct DirectionalLight;
    friend struct SpotLight;
};

}