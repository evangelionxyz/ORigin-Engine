// Copyright (c) Evangelion Manuhutu | Origin Engine

#ifndef TEXTURE_TYPE_HPP
#define TEXTURE_TYPE_HPP

#include "Origin/Core/Types.h"
#include "Texture.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace origin {

enum class TextureType : u8
{
    NONE,
    DIFFUSE,
    SPECULAR,
    AMBIENT,
    EMISSIVE,
    HEIGHT,
    NORMALS,
    SHININESS,
    OPACITY,
    DISPLACEMENT,
    LIGHTMAP,
    REFLECTION,
    BASE_COLOR,
    NORMAL_CAMERA,
    EMISSION_COLOR,
    METALNESS,
    AMBIENT_OCCLUSION,
    DIFFUSE_ROUGHNESS,
};

using TextureTypeMap = std::unordered_map<TextureType, Ref<Texture2D>>;

static aiTextureType ToAssimpTexture(TextureType type)
{
    switch (type)
    {
    case TextureType::NONE: return aiTextureType_NONE;
    case TextureType::DIFFUSE: return aiTextureType_DIFFUSE;
    case TextureType::SPECULAR: return aiTextureType_SPECULAR;
    case TextureType::AMBIENT: return aiTextureType_AMBIENT;
    case TextureType::EMISSIVE: return aiTextureType_EMISSIVE;
    case TextureType::HEIGHT: return aiTextureType_HEIGHT;
    case TextureType::NORMALS: return aiTextureType_NORMALS;
    case TextureType::SHININESS: return aiTextureType_SHININESS;
    case TextureType::OPACITY: return aiTextureType_OPACITY;
    case TextureType::DISPLACEMENT: return aiTextureType_DISPLACEMENT;
    case TextureType::LIGHTMAP: return aiTextureType_LIGHTMAP;
    case TextureType::REFLECTION: return aiTextureType_REFLECTION;
    case TextureType::BASE_COLOR: return aiTextureType_BASE_COLOR;
    case TextureType::NORMAL_CAMERA: return aiTextureType_NORMAL_CAMERA;
    case TextureType::EMISSION_COLOR: return aiTextureType_EMISSION_COLOR;
    case TextureType::METALNESS: return aiTextureType_METALNESS;
    case TextureType::AMBIENT_OCCLUSION: return aiTextureType_AMBIENT_OCCLUSION;
    case TextureType::DIFFUSE_ROUGHNESS: return aiTextureType_DIFFUSE_ROUGHNESS;
    }
    return aiTextureType_NONE;
}

static TextureType FromAssimpTexture(aiTextureType type)
{
    switch (type)
    {
    case aiTextureType_NONE: return TextureType::NONE;
    case aiTextureType_DIFFUSE: return TextureType::DIFFUSE;
    case aiTextureType_SPECULAR: return TextureType::SPECULAR;
    case aiTextureType_AMBIENT: return TextureType::AMBIENT;
    case aiTextureType_EMISSIVE: return TextureType::EMISSIVE;
    case aiTextureType_HEIGHT: return TextureType::HEIGHT;
    case aiTextureType_NORMALS: return TextureType::NORMALS;
    case aiTextureType_SHININESS: return TextureType::SHININESS;
    case aiTextureType_OPACITY: return TextureType::OPACITY;
    case aiTextureType_DISPLACEMENT: return TextureType::DISPLACEMENT;
    case aiTextureType_LIGHTMAP: return TextureType::LIGHTMAP;
    case aiTextureType_REFLECTION: return TextureType::REFLECTION;
    case aiTextureType_BASE_COLOR: return TextureType::BASE_COLOR;
    case aiTextureType_NORMAL_CAMERA: return TextureType::NORMAL_CAMERA;
    case aiTextureType_EMISSION_COLOR: return TextureType::EMISSION_COLOR;
    case aiTextureType_METALNESS: return TextureType::METALNESS;
    case aiTextureType_AMBIENT_OCCLUSION: return TextureType::AMBIENT_OCCLUSION;
    case aiTextureType_DIFFUSE_ROUGHNESS: return TextureType::DIFFUSE_ROUGHNESS;
    }
    return TextureType::NONE;
}

}

#endif
