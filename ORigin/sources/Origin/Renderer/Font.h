// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Asset\Asset.h"
#include "Texture.h"

#include "msdf-atlas-gen.h"

namespace origin {

	

	class Font : public Asset
	{
	public:
		struct Data
		{
			std::vector<msdf_atlas::GlyphGeometry> Glyphs;
			msdf_atlas::FontGeometry FontGeometry;
			TextureSpecification TexSpec;
			Buffer Buffer;
			std::string Filepath;
			int Width = 0, Height = 0;
		};

		Font() = default;
		Font(Font::Data *data);
		~Font();

		const Data *GetData() const { return m_Data; }
		std::string GetFilepath() const { return m_Data->Filepath; }
		const std::shared_ptr<Texture2D> &GetAtlasTexture() const { return m_AtlasTexture; }

		static void LoadFont(std::vector<Data *> *fontsData, std::filesystem::path filepath);
		static Data *LoadFontData(const std::filesystem::path &filepath);
		
		bool IsLoaded = false;

		static AssetType GetStaticType() { return AssetType::Font; }
		AssetType GetType() const override { return GetStaticType(); }

	private:
		Data *m_Data = nullptr;
		std::shared_ptr<Texture2D> m_AtlasTexture;
	};
}