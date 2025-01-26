// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Lighting.hpp"
#include "Origin/Renderer/Renderer.h"

namespace origin {

DirectionalLight::DirectionalLight()
{
    Renderer::lighting_manager->directional_light_count++;
}

DirectionalLight::~DirectionalLight()
{
    Renderer::lighting_manager->directional_light_count--;
}

void DirectionalLight::Bind() const
{
    Renderer::lighting_manager->directional_uniform_buffer->Bind();
    Renderer::lighting_manager->directional_uniform_buffer->SetData(&color, DIRECTIONAL_LIGHT_BUFFER_SIZE, 0);
    Renderer::lighting_manager->directional_uniform_buffer->SetData(&direction, DIRECTIONAL_LIGHT_BUFFER_SIZE, sizeof(glm::vec3));
}

void DirectionalLight::Unbind() const
{
    Renderer::lighting_manager->directional_uniform_buffer->Unbind();
}

}
