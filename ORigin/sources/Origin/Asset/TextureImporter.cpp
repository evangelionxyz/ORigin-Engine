// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "TextureImporter.h"

#include "Origin\Project\Project.h"
#include "Origin\Renderer\Buffer.h"
#include "AssetManager.h"
#include "stb_image.h"

namespace origin {

	std::shared_ptr<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadTexture2D(Project::GetActiveAssetDirectory() / metadata.Filepath);
	}

	std::shared_ptr<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& path)
	{
		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);

		Buffer data;
		data.Data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			OGN_CORE_ERROR("TextureImporter::ImpprtTexture2D - Could not load texture from filepath: {}", path.string());
			return nullptr;
		}

		data.Size = width * height * channels;
		TextureSpecification spec;
		spec.Width = width;
		spec.Height = height;

		switch (channels)
		{
		default:
		case 3:
			spec.Format = ImageFormat::RGB8;
			break;
		case 4:
			spec.Format = ImageFormat::RGBA8;
			break;
		}

		std::shared_ptr<Texture2D> texture = Texture2D::Create(spec, data);
		data.Release();

		return texture;
	}

	std::shared_ptr<Texture2D> TextureImporter::GetWhiteTexture()
	{
		static std::shared_ptr<Texture2D> whiteTexture;
		if (!whiteTexture)
		{
			uint32_t whiteTextureData = 0xffffffff;
			Buffer data = Buffer(&whiteTextureData, sizeof(uint32_t));

			whiteTexture= Texture2D::Create(TextureSpecification(), data);
		}

		return whiteTexture;
	}

}
