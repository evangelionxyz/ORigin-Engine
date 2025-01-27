// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "MaterialManager.hpp"
#include "MaterialBufferData.hpp"
#include "Origin/Renderer/Renderer.h"

#include <glad/glad.h>

namespace origin {
static MaterialManager *s_material_manager;

MaterialManager::MaterialManager()
{
    s_material_manager = this;
}

void MaterialManager::CreateMaterialStorageBuffer()
{
    glGenBuffers(1, &m_SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, m_material_buffer_size_allocated, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

size_t MaterialManager::AddMaterial(MaterialBufferData &buffer)
{
    if (s_material_manager->m_material_count * sizeof(MaterialBufferData) >= s_material_manager->m_material_buffer_size_allocated)
    {
        // double the buffer size
        size_t new_capacity = s_material_manager->m_material_count * 2;
        size_t new_buffer_size = new_capacity * sizeof(MaterialBufferData);

        // create a new buffer with the updated size
        u32 new_ssbo;
        glGenBuffers(1, &new_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, new_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, new_buffer_size, nullptr, GL_DYNAMIC_DRAW);

        // copy existing data to the new buffer
        glBindBuffer(GL_COPY_READ_BUFFER, s_material_manager->m_SSBO);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, 0, 0, s_material_manager->m_material_buffer_size_allocated);

        // delete the old buffer and update the reference
        glDeleteBuffers(1, &s_material_manager->m_SSBO);
        s_material_manager->m_SSBO = new_ssbo;
        s_material_manager->m_material_buffer_size_allocated = new_buffer_size;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, s_material_manager->m_SSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, s_material_manager->m_material_count * sizeof(MaterialBufferData), sizeof(MaterialBufferData), &buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return s_material_manager->m_material_count++;
}

void MaterialManager::UpdateMaterial(size_t index, const MaterialBufferData &updated_buffer)
{
    if (index >= s_material_manager->m_material_count)
        return;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, s_material_manager->m_SSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, index * sizeof(MaterialBufferData), sizeof(MaterialBufferData), &updated_buffer);
}

}
