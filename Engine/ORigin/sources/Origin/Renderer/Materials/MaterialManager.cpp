// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "MaterialManager.hpp"
#include "MaterialBufferData.hpp"
#include "Origin/Renderer/Renderer.h"

#include <glad/glad.h>

namespace origin {


MaterialManager::MaterialManager()
{
}

void MaterialManager::CreateMeshMaterialSSBO()
{
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, mesh_material_buffer_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

size_t MaterialManager::AddMeshMaterial(MeshMaterialBufferData &buffer)
{
    if (mesh_material_count * sizeof(MeshMaterialBufferData) >= mesh_material_buffer_size)
    {
        // double the buffer size
        size_t new_capacity = mesh_material_count * 2;
        size_t new_buffer_size = new_capacity * sizeof(MeshMaterialBufferData);

        // create a new buffer with the updated size
        u32 new_ssbo;
        glGenBuffers(1, &new_ssbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, new_ssbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, new_buffer_size, nullptr, GL_DYNAMIC_DRAW);

        // copy existing data to the new buffer
        glBindBuffer(GL_COPY_READ_BUFFER, ssbo);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_SHADER_STORAGE_BUFFER, 0, 0, mesh_material_buffer_size);

        // delete the old buffer and update the reference
        glDeleteBuffers(1, &ssbo);
        ssbo = new_ssbo;
        mesh_material_buffer_size = new_buffer_size;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, mesh_material_count * sizeof(MeshMaterialBufferData), sizeof(MeshMaterialBufferData), &buffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return mesh_material_count++;
}

void MaterialManager::UpdateMeshMaterial(size_t index, const MeshMaterialBufferData &updatedBuffer)
{
    if (index >= mesh_material_count)
        return;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, index * sizeof(MeshMaterialBufferData), sizeof(MeshMaterialBufferData), &updatedBuffer);
}

}
