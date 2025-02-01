#include "pch.h"
#include "LightingManager.hpp"
#include "Origin/Renderer/Renderer.h"

#include <glad/glad.h>

namespace origin {

static LightingManager *s_instance = nullptr;

LightingManager::LightingManager()
{
    s_instance = this;
    
    lighting_uniform_buffer = UniformBuffer::Create(sizeof(LightingManager::Data), UNIFORM_LIGHTING_BINDING);
    directional_uniform_buffer = UniformBuffer::Create(sizeof(DirectionalLight::Data), UNIFORM_DIRECTIONAL_LIGHT_BINDING);
}

void LightingManager::Bind() const
{
    // lighting uniform buffer
    lighting_uniform_buffer->Bind();
    lighting_uniform_buffer->SetData(&m_data, sizeof(LightingManager::Data));
}

void LightingManager::Unbind() const
{
    lighting_uniform_buffer->Unbind();
}

void LightingManager::CreateStorageBuffer()
{
    CreateSpotLightStorageBuffer();
    CreatePointLightStorageBuffer();
}

void LightingManager::AddPointLight(const PointLight::Data &buffer_data)
{
    if (m_data.point_light_count * sizeof(PointLight::Data) >= pointlight_buffer_size)
    {
        // double the buffer size
        pointlight_capcity = m_data.point_light_count * 2;
        size_t new_buffer_size = pointlight_capcity * sizeof(PointLight::Data);

        // create a new buffer with the updated size
        u32 new_ssbo;
        glGenBuffers(1, &new_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, new_ssbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, STORAGE_BUFFER_POINTLIGHT_BINDING, new_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, new_buffer_size, nullptr, GL_DYNAMIC_DRAW);

        // copy existing data to the new buffer
        glBindBuffer(GL_COPY_READ_BUFFER, pointlight_ssbo);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, 0, 0, pointlight_buffer_size);

        // delete the old buffer and update the reference
        glDeleteBuffers(1, &pointlight_ssbo);
        pointlight_ssbo = new_ssbo;
        pointlight_buffer_size = new_buffer_size;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pointlight_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, STORAGE_BUFFER_POINTLIGHT_BINDING, pointlight_ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, m_data.point_light_count * sizeof(PointLight::Data), sizeof(PointLight::Data), &buffer_data);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingManager::UpdatePointLight(size_t index, const PointLight::Data &updated_buffer)
{
    if (index >= m_data.point_light_count)
        return;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pointlight_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, STORAGE_BUFFER_POINTLIGHT_BINDING, pointlight_ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, index * sizeof(PointLight::Data), sizeof(PointLight::Data), &updated_buffer);
}

LightingManager* LightingManager::GetInstance()
{
    return s_instance;    
}

void LightingManager::CreatePointLightStorageBuffer()
{
    glGenBuffers(1, &pointlight_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pointlight_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, pointlight_buffer_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, STORAGE_BUFFER_POINTLIGHT_BINDING, pointlight_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void LightingManager::CreateSpotLightStorageBuffer()
{
    glGenBuffers(1, &m_spotlight_SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_spotlight_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, spotlight_buffer_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, STORAGE_BUFFER_SPOTLIGHT_BINDING, m_spotlight_SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

}
