// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef MESH_MATERIAL_HPP
#define MESH_MATERIAL_HPP

#include "Origin/Core/Types.h"
#include "Origin/Math/Math.h"
#include "Origin/Renderer/TextureType.hpp"

#include <assimp/scene.h>
#include <unordered_map>

namespace origin {

class Texture2D;
class Shader;

class MeshMaterial
{
public:
    std::vector<TextureTypeMap> textures;
    Ref<Shader> shader;

    glm::vec3 diffuse_color;
    float metallic_factor;
    float roughness_factor;

    MeshMaterial()
        : metallic_factor(1.0f), 
        roughness_factor(0.0f), 
        diffuse_color(1.0f)
    {
    }
};

}

#endif