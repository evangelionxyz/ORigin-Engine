// Copyright (c) Evangelion Manuhutu | Origin Engine

#pragma once
#include "Material.hpp"
#include "Origin/Renderer/UniformBuffer.h"

namespace origin {

struct MaterialManager
{
    MaterialManager();
    void CreateMaterialStorageBuffer();
    
    static size_t AddMaterial(MaterialBufferData &buffer);
    static void UpdateMaterial(size_t index, const MaterialBufferData &updated_buffer);

private:
    size_t m_initial_capcity = 10;
    size_t m_material_count = 0;
    size_t m_material_buffer_size_allocated = m_initial_capcity * sizeof(MaterialBufferData);
    u32 m_SSBO;
};

}
