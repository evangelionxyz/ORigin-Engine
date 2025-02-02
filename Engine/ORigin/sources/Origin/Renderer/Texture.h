// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <memory>
#include <assimp/texture.h>

#include "Origin/Asset/Asset.h"
#include "Origin/Core/Buffer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <filesystem>


namespace origin {

enum class ImageWrapMode
{
	CLAMP_TO_EDGE,
	REPEAT
};

enum class ImageFormat
{
	Invalid,
	R8, 
	RG8, 
	RGB8, 
	RGBA8, 
	RGBA32F
};

enum class ImageFilter
{
	Linear, Nearest,
	LinearMipmapLinear, LinearMipmapNearest,
	NearestMipmapLinear, NearestMipmapNearest
};

struct TextureSpecification
{
	u32 width = 1, height = 1;
	ImageFormat format = ImageFormat::RGBA8;
	ImageFilter min_filter = ImageFilter::Nearest;
	ImageFilter mag_filter = ImageFilter::Nearest;

	bool generate_mips = false;

	TextureSpecification() = default;
};

static ImageFormat GetImageFormat(i32 channels)
{
    switch (channels)
    {
    case 1: return ImageFormat::R8;
    case 2: return ImageFormat::RG8;
    case 3: return ImageFormat::RGB8;
    case 4: return ImageFormat::RGBA8;
    }
	return ImageFormat::Invalid;
}

class Texture : public Asset
{
public:
	Texture() = default;
	virtual ~Texture() {};

	virtual void SetData(Buffer data) {}

	virtual void Bind(u32 binding_point, u32 index, u32 array_count) {}
	virtual void Bind(u32 slot = 0) {}
	virtual void Unbind() {}
	virtual void Destroy() {}

    u32 GetWidth() const { return m_width; }
    u32 GetHeight() const { return m_height; }

	std::filesystem::path GetFilepath() const { return m_filepath; };
	std::string GetName() const { return m_filepath.filename().string(); }
	
    bool IsLoaded() const { return m_is_loaded; }
    const TextureSpecification &GetSpecification() const { return m_spec; }

	virtual void ChangeSize(const i32 width, const i32 height) {}
	virtual bool operator==(const Texture &other) const = 0;

    u64 GetEstimatedSize() const { return m_width * m_height * m_channels; }
	u32 GetID() const { return m_id; }
	bool IsTransparent() const { return m_is_transparent; }

protected:
    bool m_is_loaded = false;
	bool m_is_transparent = false;

	u32 m_id = 0;
    u32 m_width, m_height, m_channels;
	TextureSpecification m_spec;

	std::filesystem::path m_filepath;
};

class Texture2D : public Texture
{
public:
	Texture2D() = default;

	static Ref<Texture2D> Create(const TextureSpecification &specification, Buffer data = Buffer());
	static Ref<Texture2D> Create(const std::filesystem::path &filepath, const TextureSpecification &specification = TextureSpecification());
	static Ref<Texture2D> Create(const aiTexture *embedded_texture);

	static AssetType GetStaticType() { return AssetType::Texture; }
	AssetType GetType() const override { return GetStaticType(); }
};

}

#endif