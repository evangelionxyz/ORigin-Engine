﻿// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Lighting.hpp"
#include "Origin/Renderer/Renderer.h"

namespace origin {

DirectionalLight::DirectionalLight()
{
    LightingManager::GetInstance()->m_data.directional_light_count++;
}

DirectionalLight::~DirectionalLight()
{
    LightingManager::GetInstance()->m_data.directional_light_count--;
}

void DirectionalLight::Bind() const
{
    LightingManager::GetInstance()->directional_uniform_buffer->Bind();
    LightingManager::GetInstance()->directional_uniform_buffer->SetData(&data, sizeof(DirectionalLight::Data));
}

void DirectionalLight::Unbind() const
{
    LightingManager::GetInstance()->directional_uniform_buffer->Unbind();
}


// ======================================
//               SPOT LIGHT
// ======================================
SpotLight::SpotLight()
{
    index = LightingManager::GetInstance()->m_data.spot_light_count++;
}

SpotLight::~SpotLight()
{
    LightingManager::GetInstance()->m_data.spot_light_count--;
}

// ======================================
//             POINT LIGHT
// ======================================
PointLight::PointLight()
{
    index = LightingManager::GetInstance()->m_data.point_light_count++;
}

PointLight::~PointLight()
{
    LightingManager::GetInstance()->m_data.point_light_count--;
    index = -1;
}


}
