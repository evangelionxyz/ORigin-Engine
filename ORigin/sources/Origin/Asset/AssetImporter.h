// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "AssetMetadata.h"

#include "Origin\Audio\AudioSource.h"
#include "Origin\Scene\SpriteSheet.h"
#include "Origin\Renderer\Texture.h"
#include "Origin\Renderer\Model.h"

namespace origin {

	class Scene;

	using AssetImportFunction = std::function<std::shared_ptr<Asset>(AssetHandle, const AssetMetadata &)>;

	class AssetImporter
	{
	public:
		static std::shared_ptr<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
		static void StartThread();
		static void CheckChanges();
	};

	class FontImporter
	{
	public:
		static std::shared_ptr<Asset> Import(AssetHandle handle, AssetMetadata metadata);
		static void LoadAsync(std::shared_ptr<Asset> *font, const std::filesystem::path &filepath,  UUID uuid = 0);

	private:
		static void LoadFont(std::vector<Font::Data *> *fontsData, const std::filesystem::path &filepath);

		static std::vector<Font::Data *> FontDatas;

		static std::vector<std::pair<std::shared_ptr<Asset> *, UUID>> Fonts;

		friend class AssetImporter;
	};

	class AudioImporter
	{
	public:
		static std::shared_ptr<AudioSource> Import(AssetHandle handle, AssetMetadata metadata);
		static std::shared_ptr<AudioSource> LoadAudioSource(const std::filesystem::path &filepath);
		static std::shared_ptr<AudioSource> LoadStreamingSource(const std::filesystem::path &filepath);
	};

	class SceneImporter
	{
	public:
		static std::shared_ptr<Scene> Import(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Scene> LoadScene(const std::filesystem::path &filepath);
		static AssetHandle OpenScene(const std::filesystem::path &filepath);
		static void SaveScene(std::shared_ptr<Scene> scene, const std::filesystem::path &path);
	};

	class TextureImporter
	{
	public:
		static std::shared_ptr<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
		static std::shared_ptr<Texture2D> LoadTexture2D(const std::filesystem::path &path);
	};

	class ModelImporter
	{
	public:
		static std::shared_ptr<Model> Import(AssetHandle handle, const AssetMetadata &metadata);
		static std::shared_ptr<Model> Load(const std::filesystem::path& path);
	};

	class SpriteSheetImporter
	{
	public:
		static std::shared_ptr<SpriteSheet> Import(AssetHandle handle, const AssetMetadata &metadata);
		static std::shared_ptr<SpriteSheet> Load(const std::filesystem::path &filepath);
	};

	class MaterialImporter
	{
	public:
		static std::shared_ptr<Material> Import(AssetHandle handle, const AssetMetadata &metadata);
		static std::shared_ptr<Material> Load(const std::filesystem::path &filepath);
	};

}
