// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Asset.h"
#include "AssetImporter.h"
#include "Origin\Core\Application.h"
#include "Origin\Project\Project.h"
#include "Origin\Serializer\MaterialSerializer.h"
#include "Origin\Serializer\SceneSerializer.h"
#include "Origin\Utils\PlatformUtils.h"
#include "Origin\Renderer\Material.h"
#include "Origin\Renderer\Renderer.h"

#include "stb_image.h"

namespace origin {

	static std::vector<std::thread> s_AssetThreads;
	static std::mutex s_AssetMutex;
	static std::condition_variable s_CV;

	// Functions to Importing and Loading assets
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{	AssetType::Audio, AudioImporter::Import },
		{	AssetType::Texture, TextureImporter::ImportTexture2D },
		{	AssetType::Scene, SceneImporter::Import },
		{ AssetType::MeshSource, ModelImporter::Import },
		{ AssetType::Material, MaterialImporter::Import },
		{ AssetType::Model, ModelImporter::Import },
		{ AssetType::Font, FontImporter::Import },
		{ AssetType::SpritesSheet, SpriteSheetImporter::Import }
	};
	
	// It is automatically match Functions by checking the metadata
	// AssetHandle handle = desiredAssetHandle
	std::shared_ptr<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			OGN_CORE_ERROR("[AssetImporter] No Importer available for asset type: {} ", AssetTypeToString(metadata.Type));
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}

	void AssetImporter::StartThread()
	{
		std::thread checker([]
		{
			while (true)
			{
				AssetImporter::CheckChanges();
				std::this_thread::sleep_for(std::chrono::milliseconds(1500));
			}
		});

		checker.detach();
	}

	void AssetImporter::CheckChanges()
	{
		std::unique_lock<std::mutex> lock(s_AssetMutex);
		s_CV.wait(lock, [] { return !FontImporter::FontDatas.empty()
			&& (FontImporter::FontDatas.size() == FontImporter::Fonts.size()); });

		OGN_CORE_TRACE("Total loaded fonts {}", FontImporter::Fonts.size());

		std::vector<int> indicesToRemove;
		std::vector<std::pair<std::shared_ptr<Asset> *, UUID>> fontsToUpdate;
		std::vector<Font::Data *> dataToUpdate;

		for (int i = 0; i < FontImporter::FontDatas.size(); ++i)
		{
			const auto &font = FontImporter::Fonts[i];

			const auto &data = FontImporter::FontDatas[i];

			if (data)
			{
				fontsToUpdate.push_back(font);
				dataToUpdate.push_back(data);
				indicesToRemove.push_back(i);
			}
		}
		
		if (!fontsToUpdate.empty())
		{
			// Submit a single task to the main thread with all the changes
			Application::Get().SubmitToMainThread([=, indicesToRemove = std::move(indicesToRemove),
				fontsToUpdate = std::move(fontsToUpdate), dataToUpdate = std::move(dataToUpdate)]() mutable
			{
				for (int i = 0; i < static_cast<int>(fontsToUpdate.size()); i++)
				{
					std::shared_ptr<Font> font = std::make_shared<Font>(dataToUpdate[i]);
					*fontsToUpdate[i].first = std::static_pointer_cast<Asset>(font);
					fontsToUpdate[i].first->get()->Handle = fontsToUpdate[i].second;
				}

				for (int i = static_cast<int>(indicesToRemove.size()) - 1; i >= 0; i--)
				{
					int index = indicesToRemove[i];
					FontImporter::FontDatas.erase(FontImporter::FontDatas.begin() + index);
					FontImporter::Fonts.erase(FontImporter::Fonts.begin() + index);
					s_AssetThreads.erase(s_AssetThreads.begin() + index);
				}
			});
		}
	}

	std::vector<Font::Data *> FontImporter::FontDatas;
	std::vector<std::pair<std::shared_ptr<Asset> *, UUID>> FontImporter::Fonts;

	std::shared_ptr<Asset> FontImporter::Import(AssetHandle handle, AssetMetadata metadata)
	{
		auto filepath = /*Project::GetActiveAssetDirectory() /*/ metadata.Filepath;

		std::shared_ptr<Asset> font;
		FontImporter::LoadAsync(&font, filepath);

		return font;
	}

	void FontImporter::LoadAsync(std::shared_ptr<Asset> *font, const std::filesystem::path &filepath, UUID uuid)
	{
		{
			std::unique_lock<std::mutex> lock(s_AssetMutex);
			FontImporter::Fonts.push_back({font, uuid });
		}
		
		s_AssetThreads.push_back(std::thread(FontImporter::LoadFont, &FontImporter::FontDatas, filepath));
		s_AssetThreads.back().detach();
	}

	void FontImporter::LoadFont(std::vector<Font::Data *> *fontsData, const std::filesystem::path &filepath)
	{
		Font::Data *data = Font::LoadFontData(filepath);
		{
			std::unique_lock<std::mutex> lock(s_AssetMutex);
			fontsData->push_back(data);
		}
		s_CV.notify_one();
	}
	
	std::shared_ptr<AudioSource> AudioImporter::Import(AssetHandle handle, AssetMetadata metadata)
	{
		return LoadAudioSource(Project::GetActiveAssetDirectory() / metadata.Filepath);
	}

	std::shared_ptr<AudioSource> AudioImporter::LoadAudioSource(const std::filesystem::path &filepath)
	{
		std::shared_ptr<AudioSource> source = AudioSource::Create();
		source->LoadSource("Audio", filepath, false, false);
		return source;
	}

	std::shared_ptr<AudioSource> AudioImporter::LoadStreamingSource(const std::filesystem::path &filepath)
	{
		std::shared_ptr<AudioSource> source = AudioSource::Create();
		source->LoadStreamingSource("Streaming Audio", filepath, false, false);
		return source;
	}

	std::shared_ptr<Scene> SceneImporter::Import(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadScene(Project::GetActiveAssetDirectory() / metadata.Filepath);
	}

	std::shared_ptr<Scene> SceneImporter::LoadScene(const std::filesystem::path& filepath)
	{
		std::shared_ptr<Scene> scene = std::make_shared<Scene>();
		SceneSerializer serializer(scene);
		serializer.Deserialize(filepath);
		return scene;
	}

	AssetHandle SceneImporter::OpenScene(const std::filesystem::path& filepath)
	{
		auto relativePath = std::filesystem::relative(filepath, Project::GetActiveAssetDirectory());

		if (!relativePath.empty())
		{
			auto &assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
			for (auto a : assetRegistry)
			{
				auto assetRelativePath = std::filesystem::relative(a.second.Filepath, Project::GetActiveAssetDirectory());
				if (relativePath == assetRelativePath)
					return a.first;
			}
		}

		// You can continue the program
		OGN_CORE_ASSERT(false, "[SceneImporter] Scene shoule be imported first via Content Browser");
		return 0;
	}

	void SceneImporter::SaveScene(std::shared_ptr<Scene> scene, const std::filesystem::path& path)
	{
		SceneSerializer serializer(scene);
		serializer.Serialize(Project::GetActiveAssetDirectory() / path);
	}

	std::shared_ptr<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadTexture2D(Project::GetActiveAssetDirectory() / metadata.Filepath);
	}

	std::shared_ptr<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path &filepath)
	{
		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);

		Buffer data;
		data.Data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);

		if (!data || channels < 3)
		{
			data.Release();

			OGN_CORE_ERROR("[ImportTexture2D] Could not load texture from filepath: {}", filepath.string());
			return nullptr;
		}

		data.Size = width * height * channels;

		TextureSpecification spec;
		spec.Width = width;
		spec.Height = height;
		spec.MinFilter = ImageFilter::Nearest;
		spec.MagFilter = ImageFilter::Nearest;

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

	std::shared_ptr<Model> ModelImporter::Import(AssetHandle handle, const AssetMetadata& metadata)
	{
		return Load(Project::GetActiveAssetDirectory() / metadata.Filepath);
	}

	std::shared_ptr<Model> ModelImporter::Load(const std::filesystem::path &filepath)
	{
		OGN_CORE_ERROR("[ModelImporter] Not implemented");
		return nullptr;
	}

	std::shared_ptr<SpriteSheet> SpriteSheetImporter::Import(AssetHandle handle, const AssetMetadata &metadata)
	{
		return Load(Project::GetActiveAssetDirectory() / metadata.Filepath);
	}

	std::shared_ptr<SpriteSheet> SpriteSheetImporter::Load(const std::filesystem::path &filepath)
	{
		return SpriteSheet::Create(filepath);
	}

	std::shared_ptr<Material> MaterialImporter::Import(AssetHandle handle, const AssetMetadata &metadata)
	{
		return Load(Project::GetActiveAssetDirectory() / metadata.Filepath);
	}

	std::shared_ptr<Material> MaterialImporter::Load(const std::filesystem::path &filepath)
	{
		std::shared_ptr<Material> material = Material::Create();
		MaterialSerializer::Deserialize(filepath, material);
		return material;
	}

	

}
