// Copyright (c) Evangelion Manuhutu | Origin Engine

#include "pch.h"
#include "Material.hpp"
#include "Origin/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <glad/glad.h>

namespace origin {

Material::~Material() {}

MeshMaterial::MeshMaterial()
{
}

void MeshMaterial::Update(Shader* shader)
{

    if (diffuse_texture)
    {
        diffuse_texture->Bind(DIFFUSE_TEXTURE_BINDING);
        shader->SetInt("udiffuse_texture", DIFFUSE_TEXTURE_BINDING);
    }
    if (specular_texture)
    {
        specular_texture->Bind(SPECULAR_TEXTURE_BINDING);
        shader->SetInt("uspecular_texture", SPECULAR_TEXTURE_BINDING);
    }
    if (roughness_texture)
    {
        roughness_texture->Bind(ROUGHNESS_TEXTURE_BINDING);
        shader->SetInt("uroughness_texture", ROUGHNESS_TEXTURE_BINDING);
    }
}

}
