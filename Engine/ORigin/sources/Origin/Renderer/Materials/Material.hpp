// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef MESH_MATERIAL_HPP
#define MESH_MATERIAL_HPP

#include "Origin/Core/Types.h"
#include "Origin/Math/Math.hpp"
#include "Origin/Renderer/TextureType.hpp"
#include "MaterialBufferData.hpp"
#include <assimp/scene.h>
#include <unordered_map>

namespace origin {

class Texture2D;
class Shader;

class Material : public Asset
{
public:
    virtual ~Material();
    
    template<typename MaterialType>
    static Ref<Material> Create()
    {
        return CreateRef<MaterialType>();
    }
    
    static AssetType GetStaticType() { return AssetType::Material; }
    AssetType GetType() const override { return GetStaticType(); }
};

class MeshMaterial : public Material
{
public:
    MeshMaterial();

    Ref<Texture2D> diffuse_texture;
    Ref<Texture2D> specular_texture;
    Ref<Texture2D> roughness_texture;
    MaterialBufferData buffer_data;

    u32 subroutine = 1;
    u32 reflective_texture;

    // 1 planar, 2 cube reflection
    i32 reflective_type = 0;
    glm::vec4 plane_or_position;

    bool reflective = false;
    bool transparent = false;

    void Update(Shader *shader);
};

}

#endif