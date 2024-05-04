// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Origin\Asset\Asset.h"
#include "Texture.h"

namespace origin {

	struct MSDFData;

	class Font : public Asset
	{
	public:
		Font(const std::filesystem::path& filepath);
		~Font();

		const MSDFData* GetMSDFData() const { return m_Data; }
		const std::shared_ptr<Texture2D>& GetAtlasTexture() const { return m_AtlasTexture; }
		std::string GetFilepath() const { return m_Filepath; }
		bool IsLoaded() const { return m_Loaded; }
		
		static std::shared_ptr<Font> Create(const std::filesystem::path &filepath);

		static std::shared_ptr<Font> GetDefault();
		static AssetType GetStaticType() { return AssetType::Font; }
		AssetType GetType() const override { return GetStaticType(); }
		
	private:
		// TODO: Add loaded fonts for an optimization

		MSDFData* m_Data;
		std::shared_ptr<Texture2D> m_AtlasTexture;
		bool m_Loaded = false;
		std::string m_Filepath;
	};
}