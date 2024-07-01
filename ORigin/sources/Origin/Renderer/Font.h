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

		Font();
		Font(FontData *data);

		~Font();

		static std::shared_ptr<Font> Create(FontData *data);
		static FontData *LoadGlyphs(const std::filesystem::path &filepath);

		const FontData *GetData() const { return m_Data; }
		std::string GetFilepath() const { return m_Data->Filepath; }
		const std::shared_ptr<Texture2D> &GetAtlasTexture() const { return m_AtlasTexture; }
		
		bool IsLoaded = false;

		static AssetType GetStaticType() { return AssetType::Font; }
		AssetType GetType() const override { return GetStaticType(); }

	private:

		FontData *m_Data = nullptr;
		std::shared_ptr<Texture2D> m_AtlasTexture;
	};
}