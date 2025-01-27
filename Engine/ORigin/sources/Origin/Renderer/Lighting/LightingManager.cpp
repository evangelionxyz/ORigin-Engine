#include "pch.h"
#include "LightingManager.hpp"
#include "Origin/Renderer/Renderer.h"

#include <glad/glad.h>

namespace origin {

static LightingManager *s_instance = nullptr;

LightingManager::LightingManager()
{
    s_instance = this;
    
    lighting_uniform_buffer = UniformBuffer::Create(sizeof(LightingManager::Data), LIGHTING_BINDING);
    directional_uniform_buffer = UniformBuffer::Create(sizeof(DirectionalLight::Data), DIRECTIONAL_LIGHT_BINDING);
}

void LightingManager::Bind() const
{
    // lighting uniform buffer
    lighting_uniform_buffer->Bind();
    lighting_uniform_buffer->SetData(&m_data.directional_light_count, sizeof(LightingManager::Data), 0);
    lighting_uniform_buffer->SetData(&m_data.spot_light_count, sizeof(LightingManager::Data), 4);
    lighting_uniform_buffer->SetData(&m_data.point_light_count, sizeof(LightingManager::Data), 8);
    lighting_uniform_buffer->SetData(&m_data.area_light_count, sizeof(LightingManager::Data), 12);
}

void LightingManager::Unbind() const
{
    lighting_uniform_buffer->Unbind();
}

void LightingManager::CreateStorageBuffer()
{
    CreateSpotLightStorageBuffer();
}

void LightingManager::AddSpotLight()
{
}

LightingManager* LightingManager::GetInstance()
{
    return s_instance;    
}

void LightingManager::CreateSpotLightStorageBuffer()
{
    glGenBuffers(1, &m_spotlight_SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_spotlight_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, spotlight_buffer_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_spotlight_SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

}
