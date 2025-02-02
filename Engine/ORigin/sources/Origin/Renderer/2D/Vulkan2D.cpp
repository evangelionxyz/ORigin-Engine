// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Vulkan2D.hpp"

namespace origin {

struct Vulkan2DData
{

};

static Vulkan2DData *vk_2d_data = nullptr;

void Vulkan2D::Init()
{
    vk_2d_data = new Vulkan2DData();
}

void Vulkan2D::Shutdown()
{
    if (vk_2d_data)
    {
        delete vk_2d_data;
    }
}

void Vulkan2D::DrawQuad(const glm::vec3 &position, const glm::vec4 &color)
{

}

void Vulkan2D::StartBatch()
{

}

void Vulkan2D::Flush()
{

}

}