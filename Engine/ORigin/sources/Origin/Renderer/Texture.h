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
	R8, 
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

class Texture : public Asset
{
public:
	Texture() = default;
	virtual ~Texture() {};

	virtual void SetData(Buffer data) = 0;

	virtual std::string GetFilepath() = 0;
	virtual void Bind(u32 bindingPoint, u32 index, u32 arrayCount) = 0;
	virtual void Bind(u32 slot = 0) = 0;
	virtual void Unbind() = 0;
	virtual void Delete() = 0;

	[[nodiscard]] virtual std::string GetName() const = 0;
	u32 GetWidth() const { return m_width; }
	u32 GetHeight() const { return m_height; }
	u32 GetIndex() const { return m_index; }

    [[nodiscard]] bool IsLoaded() const { return m_is_loaded; }
	[[nodiscard]] virtual const TextureSpecification &GetSpecification() const = 0;

	[[nodiscard]] virtual u64 GetEstimatedSize() const = 0;
	virtual void ChangeSize(const i32 width, const i32 height) = 0;

	virtual bool operator==(const Texture &other) const = 0;

	operator ImTextureID() { return reinterpret_cast<ImTextureID>((uintptr_t)m_id); }
	[[nodiscard]] u32 GetID() const { return m_id; }

protected:
    bool m_is_loaded = false;
	u32 m_id = 0;
    u32 m_width, m_height, m_channels, m_index;
};

class Texture2D : public Texture
{
public:
	Texture2D() = default;

	static Ref<Texture2D> Create(const TextureSpecification &specification, Buffer data = Buffer());
	static Ref<Texture2D> Create(const std::filesystem::path &filepath, const TextureSpecification &specification = TextureSpecification());
	static Ref<Texture2D> Create(const aiTexture *embedded_texture);

	static AssetType GetStaticType() { return AssetType::Texture; }
	[[nodiscard]] AssetType GetType() const override { return GetStaticType(); }
};

class TextureCube : public Texture
{
public:
	enum Faces
	{
		RIGHT = 0, LEFT,
		TOP, BOTTOM,
		BACK, FRONT
	};

	static Ref<TextureCube> Create(u32 width, u32 height);
	static Ref<TextureCube> Create(const std::string &filepath);

	static AssetType GetStaticType() { return AssetType::Texture; }
	[[nodiscard]] AssetType GetType() const override { return GetStaticType(); }
};
}

#endif