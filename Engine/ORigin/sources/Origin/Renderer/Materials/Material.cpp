// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "Material.hpp"
#include "Origin/Renderer/Renderer.h"

#include <glad/glad.h>

namespace origin {

Material::~Material() {}

MeshMaterial::MeshMaterial()
{
}

void MeshMaterial::Bind() const
{
    Renderer::material_manager->mesh_material_uniform_buffer->Bind();
    Renderer::material_manager->mesh_material_uniform_buffer->SetData(&buffer_data, sizeof(MeshMaterialBufferData));

    GLenum error = glGetError();
    OGN_CORE_ASSERT(error == GL_NO_ERROR, "[OpenGLRendererAPI] {}", error);
}

void MeshMaterial::Unbind() const
{
    Renderer::material_manager->mesh_material_uniform_buffer->Unbind();
}


}
