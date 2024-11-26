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
		R8, RGB8, RGBA8, RGBA32F
	};

	enum class ImageFilter
	{
		Linear, Nearest,
		LinearMipmapLinear, LinearMipmapNearest,
		NearestMipmapLinear, NearestMipmapNearest
	};

	struct TextureSpecification
	{
		u32 Width = 1, Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		ImageFilter MinFilter = ImageFilter::Nearest;
		ImageFilter MagFilter = ImageFilter::Nearest;
		bool GenerateMips = false;

		TextureSpecification() = default;
	};

	class Texture : public Asset
	{
	public:
		Texture() = default;
		~Texture() {};

		virtual void SetData(Buffer data) = 0;
		[[nodiscard]] virtual const TextureSpecification& GetSpecification() const = 0;

		[[nodiscard]] virtual u32 GetWidth() const = 0;
		[[nodiscard]] virtual u32 GetHeight() const = 0;
		[[nodiscard]] virtual u32 GetIndex() const = 0;

		virtual std::string GetFilepath() = 0;
		[[nodiscard]] virtual std::string GetName() const = 0;
		virtual void Bind(u32 bindingPoint, u32 index, u32 arrayCount) = 0;
		virtual void Bind(u32 slot = 0) = 0;
		virtual void Unbind() = 0;
		virtual void Delete() = 0;
		[[nodiscard]] virtual bool IsLoaded() const = 0;

		[[nodiscard]] virtual u64 GetEstimatedSize() const = 0;
		virtual void ChangeSize(const i32 width, const i32 height) = 0;

		virtual bool operator==(const Texture& other) const = 0;

        operator ImTextureID() { return reinterpret_cast<ImTextureID>((uintptr_t)m_TextureID); }
        [[nodiscard]] u32 GetID() const { return m_TextureID; }

	protected:
		u32 m_TextureID = 0;
	};

	class Texture2D : public Texture
	{
	public:
		Texture2D() = default;

		static Ref<Texture2D> Create(const TextureSpecification& specification, Buffer data = Buffer());
		static Ref<Texture2D> Create(const std::filesystem::path& filepath,
			const TextureSpecification& specification = TextureSpecification());
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
		static Ref<TextureCube> Create(const std::string& filepath);

		static AssetType GetStaticType() { return AssetType::Texture; }
		[[nodiscard]] AssetType GetType() const override { return GetStaticType(); }
	};
}

#endif