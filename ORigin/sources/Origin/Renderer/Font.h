// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Asset\Asset.h"
#include "Texture.h"

#include "msdf-atlas-gen.h"

namespace origin {

	struct FontData
	{
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
		msdf_atlas::FontGeometry FontGeometry;
		TextureSpecification TexSpec;
		Buffer Buffer;
		std::string Filepath;
		int Width = 0, Height = 0;
	};

	class Font : public Asset
	{
	public:
		Font(FontData *data);
		~Font();

		const FontData *GetData() const { return m_Data; }
		std::string GetFilepath() const { return m_Data->Filepath; }
		const std::shared_ptr<Texture2D> &GetAtlasTexture() const { return m_AtlasTexture; }

		static AssetType GetStaticType() { return AssetType::Font; }
		AssetType GetType() const override { return GetStaticType(); }

		static void CheckChanges();
		
		bool IsLoaded() const { return m_Loaded; }
	private:
		FontData *m_Data = nullptr;
		std::shared_ptr<Texture2D> m_AtlasTexture;

		bool m_Loaded = false;
	};

	struct FontLoader
	{
		static void LoadFontAsync(std::shared_ptr<Font> *font, std::filesystem::path filepath);
	};
}