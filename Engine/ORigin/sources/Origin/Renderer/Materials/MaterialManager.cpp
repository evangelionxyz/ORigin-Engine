// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "MaterialManager.hpp"
#include "MaterialBufferData.hpp"
#include "Origin/Renderer/Renderer.h"

namespace origin {


MaterialManager::MaterialManager()
{
    mesh_material_uniform_buffer = UniformBuffer::Create(sizeof(MeshMaterialBufferData), MATERIAL_BINDING);
}

void MaterialManager::Bind() const
{
}

void MaterialManager::Unbind() const
{
}

}
