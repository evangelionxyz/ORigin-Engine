#include "pch.h"
#include "LightingManager.hpp"
#include "Origin/Renderer/Renderer.h"

namespace origin {
LightingManager::LightingManager()
{
    lighting_uniform_buffer = UniformBuffer::Create(LIGHTING_BUFFER_SIZE, LIGHTING_BINDING);
    directional_uniform_buffer = UniformBuffer::Create(DIRECTIONAL_LIGHT_BUFFER_SIZE, DIRECTIONAL_LIGHT_BINDING);
}

void LightingManager::Bind() const
{
    // lighting uniform buffer
    lighting_uniform_buffer->Bind();
    lighting_uniform_buffer->SetData(&directional_light_count, LIGHTING_BUFFER_SIZE, 0);
    lighting_uniform_buffer->SetData(&spot_light_count, LIGHTING_BUFFER_SIZE, 4);
    lighting_uniform_buffer->SetData(&point_light_count, LIGHTING_BUFFER_SIZE, 8);
    lighting_uniform_buffer->SetData(&area_light_count, LIGHTING_BUFFER_SIZE, 12);
}

void LightingManager::Unbind() const
{
    lighting_uniform_buffer->Unbind();
}

}