// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once

#include <string>
#include <memory>

#include "Origin/Asset/Asset.h"
#include "Origin/Core/Buffer.h"

#include <filesystem>

namespace origin {

	enum class ImageFormat
	{
		None = 0,
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
		uint32_t Width = 1, Height = 1;
		ImageFormat Format = ImageFormat::RGBA8;
		ImageFilter MinFilter = ImageFilter::Nearest;
		ImageFilter MagFilter = ImageFilter::Nearest;
		bool GenerateMips = false;

		TextureSpecification() = default;
	};

	class Texture : public Asset
	{
	public:
		virtual ~Texture() = default;
		virtual void SetData(Buffer data) = 0;
		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetIndex() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual std::string GetFilepath() = 0;
		virtual std::string GetName() const = 0;
		virtual void Bind(uint32_t bindingPoint, uint32_t index, uint32_t arrayCount) = 0;
		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void Unbind() = 0;
		virtual void Delete() = 0;
		virtual bool IsLoaded() const = 0;

		virtual uint64_t GetEstimatedSize() const = 0;
		virtual void ChangeSize(uint64_t width, uint64_t height) = 0;

		virtual void SetMaterialTypeName(const std::string& typeName) = 0;
		virtual const std::string& GetMaterialTypeName() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static std::shared_ptr<Texture2D> Create(const TextureSpecification& specification, Buffer data = Buffer());
		static std::shared_ptr<Texture2D> Create(const std::filesystem::path& filepath, const TextureSpecification& specification = TextureSpecification());
		static AssetType GetStaticType() { return AssetType::Texture; }
		virtual AssetType GetType() const { return GetStaticType(); }
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

		static std::shared_ptr<TextureCube> Create(uint32_t width, uint32_t height);
		static std::shared_ptr<TextureCube> Create(const std::string& filepath);

		static AssetType GetStaticType() { return AssetType::Texture; }
		virtual AssetType GetType() const { return GetStaticType(); }
	};
}