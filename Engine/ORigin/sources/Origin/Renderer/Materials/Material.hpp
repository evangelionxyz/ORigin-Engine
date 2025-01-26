// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef MESH_MATERIAL_HPP
#define MESH_MATERIAL_HPP

#include "Origin/Core/Types.h"
#include "Origin/Math/Math.h"
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
    
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

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

    MeshMaterialBufferData buffer_data;
    
    Ref<Shader> shader;

    void Bind() const override;
    void Unbind() const override;
    
};

}

#endif