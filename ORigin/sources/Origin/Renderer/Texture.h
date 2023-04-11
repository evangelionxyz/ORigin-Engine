// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once

#include <string>
#include <memory>

namespace Origin
{
	class Texture
	{
	public:

		virtual ~Texture() = default;
		virtual void SetData(void* data, uint32_t size) = 0;

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

	class Texture3D : public Texture
	{
	public:
		enum Faces
		{
			RIGHT = 0, LEFT,
			TOP, BOTTOM,
			BACK, FRONT
		};

		void LoadFaces(std::string& filepath, Faces faces);

		static std::shared_ptr<Texture3D> Create(uint32_t width, uint32_t height);
		static std::shared_ptr<Texture3D> Create(const std::string& filepath);
	};

	class Texture2D : public Texture
	{
	public:
		static std::shared_ptr<Texture2D> Create(uint32_t width, uint32_t height);
		static std::shared_ptr<Texture2D> Create(const std::string& filepath);
	};
}