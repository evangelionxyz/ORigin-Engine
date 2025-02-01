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
    size_t spotlight_capacity = 10;
    size_t spotlight_buffer_size = spotlight_capacity * sizeof(SpotLight::Data);
    
    size_t pointlight_capcity = 10;
    size_t pointlight_buffer_size = pointlight_capcity * sizeof(PointLight::Data);

    Ref<UniformBuffer> lighting_uniform_buffer;
    Ref<UniformBuffer> directional_uniform_buffer;

    LightingManager();
    
    void Bind() const;
    void Unbind() const;

    void CreateStorageBuffer();
    void AddPointLight(const PointLight::Data &buffer_data);
    void UpdatePointLight(size_t index, const PointLight::Data &updated_buffer);

    static LightingManager *GetInstance();

private:
    struct Data
    {
        i32 directional_light_count = 0;
        i32 spot_light_count = 0;
        i32 point_light_count = 0;
        i32 area_light_count = 0;
    };
    
    void CreatePointLightStorageBuffer();
    void CreateSpotLightStorageBuffer();

    u32 m_spotlight_SSBO;
    u32 pointlight_ssbo;
    Data m_data;

    friend struct DirectionalLight;
    friend struct SpotLight;
    friend struct PointLight;
};

}