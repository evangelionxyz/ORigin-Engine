// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once

#include <string>
#include <memory>

#include "Origin\Asset\Asset.h"

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
		ImageFilter MinFilter = ImageFilter::Linear;
		ImageFilter MagFilter = ImageFilter::Linear;
		bool GenerateMips = true;

		TextureSpecification() = default;
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual void SetData(void* data, uint32_t size) = 0;

		virtual const TextureSpecification& GetSpecification() const = 0;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetIndex() const = 0;
		virtual uint32_t GetRendererID() const = 0;
		virtual std::string GetFilepath() = 0;
		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void Delete() = 0;
		virtual bool IsLoaded() const = 0;
		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static std::shared_ptr<Texture2D> Create(const TextureSpecification& specification);
		static std::shared_ptr<Texture2D> Create(const std::string& filepath, const TextureSpecification& specification = TextureSpecification());

		static AssetType GetStaticType() { return AssetType::Texture2D; }
		virtual AssetType GetType() { return GetStaticType(); }
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

		void LoadFaces(std::string& filepath, Faces faces);
		static std::shared_ptr<TextureCube> Create(uint32_t width, uint32_t height);
		static std::shared_ptr<TextureCube> Create(const std::string& filepath);
	};
}