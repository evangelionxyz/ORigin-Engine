// Copyright (c) Evangelion Manuhutu | Origin Engine

#pragma once
#include "Material.hpp"
#include "Origin/Renderer/UniformBuffer.h"

namespace origin {

struct MaterialManager
{
    Ref<UniformBuffer> mesh_material_uniform_buffer;

    MaterialManager();

    void Bind() const;
    void Unbind() const;
};

}
