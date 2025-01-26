// Copyright (c) Evangelion Manuhutu | Origin Engine

#pragma once
#include "Material.hpp"
#include "Origin/Renderer/UniformBuffer.h"

namespace origin {

struct MaterialManager
{
    size_t initiali_capcity = 10;
    size_t mesh_material_count = 0;
    size_t mesh_material_buffer_size = initiali_capcity * sizeof(MeshMaterialBufferData);

    MaterialManager();
    
    void CreateMeshMaterialSSBO();
    size_t AddMeshMaterial(MeshMaterialBufferData &buffer);
    void UpdateMeshMaterial(size_t index, const MeshMaterialBufferData &updatedBuffer);

    std::unordered_map<std::string, i32> material_index_map;

    u32 ssbo;
};

}
