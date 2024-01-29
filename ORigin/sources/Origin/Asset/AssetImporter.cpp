// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"
#include "Asset.h"
#include "AssetImporter.h"
#include "Origin\Project\Project.h"
#include "Origin\Scene\SceneSerializer.h"
#include "Origin\Utils\PlatformUtils.h"
#include "Origin\Renderer\Material.h"
#include "Origin\Renderer\Renderer.h"

#include "stb_image.h"

namespace origin {

	using AssetImportFunction = std::function<std::shared_ptr<Asset>(AssetHandle, const AssetMetadata&)>;

	// Functions to Importing and Loading assets
	static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions = {
		{	AssetType::Audio, AudioImporter::ImportAudio },
		{	AssetType::Texture2D, TextureImporter::ImportTexture2D },
		{	AssetType::Scene, SceneImporter::ImportScene },
		{ AssetType::Mesh, ModelImporter::ImportModel },
		{ AssetType::StaticMesh, ModelImporter::ImportModel }
	};
	
	// It is automatically match Functions by checking the metadata
	// AssetHandle handle = desiredAssetHandle
	std::shared_ptr<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
	{
		if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
		{
			OGN_CORE_ERROR("No Importer available for asset type: {} ", AssetTypeToString(metadata.Type));
			return nullptr;
		}

		return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
	}

	std::shared_ptr<Audio> AudioImporter::ImportAudio(AssetHandle handle, AssetMetadata metadata)
	{
		AudioConfig config;
		config.Name = metadata.Filepath.filename().string();
		config.Spatial = false;
		config.Looping = false;

		return LoadAudio(Project::GetActiveAssetDirectory() / metadata.Filepath, config);
	}

	std::shared_ptr<Audio> AudioImporter::LoadAudio(const std::filesystem::path filepath, AudioConfig config)
	{
		std::shared_ptr<Audio> audio = Audio::Create();
		audio->LoadSource(filepath, config);

		return audio;
	}

	std::shared_ptr<Scene> SceneImporter::ImportScene(AssetHandle handle, const AssetMetadata& metadata)
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
			auto& assetRegistry = Project::GetActive()->GetEditorAssetManager()->GetAssetRegistry();
			for (auto a : assetRegistry)
			{
				if (relativePath.generic_string() == a.second.Filepath)
					return a.first;
			}
		}

		OGN_CORE_ASSERT(false, "SceneImporter: Scene shoule be imported");
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

	std::shared_ptr<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path& filepath)
	{
		int width, height, channels;

		stbi_set_flip_vertically_on_load(1);

		Buffer data;
		data.Data = stbi_load(filepath.string().c_str(), &width, &height, &channels, 0);

		if (!data)
		{
			OGN_CORE_ERROR("TextureImporter::ImpprtTexture2D - Could not load texture from filepath: {}", filepath.string());
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

	std::shared_ptr<Texture2D> TextureImporter::GetWhiteTexture()
	{
		static std::shared_ptr<Texture2D> whiteTexture;
		if (!whiteTexture)
		{
			uint32_t whiteTextureData = 0xffffffff;
			Buffer data = Buffer(&whiteTextureData, sizeof(uint32_t));

			whiteTexture = Texture2D::Create(TextureSpecification(), data);
		}

		return whiteTexture;
	}

	std::shared_ptr<Model> ModelImporter::ImportModel(AssetHandle handle, const AssetMetadata& metadata)
	{
		return LoadModel(Project::GetActiveAssetDirectory() / metadata.Filepath);
	}

	std::shared_ptr<Model> ModelImporter::LoadModel(const std::filesystem::path& path)
	{
		std::shared_ptr<Shader> shader = Shader::Create("Resources/Shaders/SPIR-V/Mesh.glsl", true);
		shader->Enable();

		std::shared_ptr<Material> material = Material::Create("model", shader);
		std::shared_ptr<Model> model = Model::Create(path.generic_string(), material);

		return model;
	}

}
