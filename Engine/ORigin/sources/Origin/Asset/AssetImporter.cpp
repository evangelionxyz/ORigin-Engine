// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"

#include "Asset.h"
#include "AssetImporter.h"
#include "Origin/Core/Application.h"
#include "Origin/Project/Project.h"
#include "Origin/Renderer/ModelLoader.h"
#include "Origin/Serializer/MaterialSerializer.h"
#include "Origin/Serializer/SceneSerializer.h"
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Renderer/Material.h"
#include "Origin/Renderer/Renderer.h"
#include "stb_image.h"

namespace origin {

    // Functions to Importing and Loading assets
    static std::map<AssetType, AssetImportFunction> s_AssetImportFunctions =
    {
        { AssetType::Audio, AudioImporter::Import },
        { AssetType::Texture, TextureImporter::ImportTexture2D },
        { AssetType::Scene, SceneImporter::Import },
        { AssetType::Material, MaterialImporter::Import },
        { AssetType::StaticMesh, ModelImporter::ImportStaticMesh },
        { AssetType::Mesh, ModelImporter::ImportMesh },
        { AssetType::Font, FontImporter::Import },
        { AssetType::SpritesSheet, SpriteSheetImporter::Import }
    };

    static AssetTaskWorker s_AssetTaskWorker;
    
    // It is automatically match Functions by checking the metadata
    // AssetHandle handle = desiredAssetHandle
    Ref<Asset> AssetImporter::ImportAsset(AssetHandle handle, const AssetMetadata& metadata)
    {
        if (s_AssetImportFunctions.find(metadata.Type) == s_AssetImportFunctions.end())
        {
            OGN_CORE_ERROR("[AssetImporter] No Importer available for asset type: {} ", AssetTypeToString(metadata.Type));
            return nullptr;
        }

        return s_AssetImportFunctions.at(metadata.Type)(handle, metadata);
    }

    void AssetImporter::SyncToMainThread(Timestep ts)
    {
        s_AssetTaskWorker.Update(ts);
    }

    void AssetTaskWorker::Update(Timestep ts)
    {
        while (!TaskQueue.empty() && this->Timer <= 0.0f)
        {
            auto &taskPtr = TaskQueue.front();
            if (taskPtr && taskPtr->Execute())
            {
                // Pop if already loaded
                TaskQueue.pop();
                this->Timer = 0.1f;
            }
            else
            {
                // Swapping
                TaskQueue.push(std::move(TaskQueue.front()));
                TaskQueue.pop();
            }
            break;
        }

        if (!TaskQueue.empty())
            this->Timer -= ts;
    }

    Ref<Font> FontImporter::Import(AssetHandle handle, AssetMetadata metadata)
    {
        auto filepath =/* Project::GetActiveAssetDirectory() /*/ metadata.Filepath;

        Ref<Font> font;
        //FontImporter::LoadAsync(&font, filepath);

        return font;
    }

    void FontImporter::LoadAsync(Ref<Asset> *font, const std::filesystem::path &filepath)
    {
        // Create the AssetTask
        AssetTask<FontData *> task(font, [](const std::filesystem::path &file)
        {
            return Font::LoadGlyphs(file); // Assuming Font::Create takes a path and returns Ref<Font>
        }, filepath);

        s_AssetTaskWorker.AddTask(task); // Move task into the worker queue
    }
    
    Ref<AudioSource> AudioImporter::Import(AssetHandle handle, AssetMetadata metadata)
    {
        return LoadAudioSource(Project::GetActiveAssetDirectory() / metadata.Filepath);
    }

    Ref<AudioSource> AudioImporter::LoadAudioSource(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
            return nullptr;

        Ref<AudioSource> source = AudioSource::Create();
        source->LoadSource("Audio", filepath, false, false);
        return source;
    }

    Ref<AudioSource> AudioImporter::LoadStreamingSource(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
            return nullptr;

        Ref<AudioSource> source = AudioSource::Create();
        source->LoadStreamingSource("Streaming Audio", filepath, false, false);
        return source;
    }

    Ref<Scene> SceneImporter::Import(AssetHandle handle, const AssetMetadata& metadata)
    {
        return LoadScene(Project::GetActiveAssetDirectory() / metadata.Filepath);
    }

    Ref<Scene> SceneImporter::LoadScene(const std::filesystem::path& filepath)
    {
        Ref<Scene> scene = CreateRef<Scene>();
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
                {
                    PUSH_CONSOLE_INFO("Scene loaded!");
                    return a.first;
                }
            }
        }

        // You can continue the program
        OGN_CORE_ASSERT(false, "[SceneImporter] Scene should be imported first via Content Browser");
        return 0;
    }

    void SceneImporter::SaveScene(Ref<Scene> scene, const std::filesystem::path& path)
    {
        SceneSerializer serializer(scene);
        auto filepath = Project::GetActiveAssetDirectory() / path;
        serializer.Serialize(filepath);
        PUSH_CONSOLE_INFO("Scene saved! {0}", filepath.generic_string());
    }

    Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
    {
        return LoadTexture2D(Project::GetActiveAssetDirectory() / metadata.Filepath);
    }

    Ref<Texture2D> TextureImporter::LoadTexture2D(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
        {
            OGN_CORE_ASSERT(false, "[Texture Importer] File '{}' does not exist", filepath.string().c_str());
            return nullptr;
        }

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
        spec.MinFilter = ImageFilter::LinearMipmapLinear;
        spec.MagFilter = ImageFilter::NearestMipmapNearest;

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

        Ref<Texture2D> texture = Texture2D::Create(spec, data);
        data.Release();

        return texture;
    }

    Ref<StaticMeshData> ModelImporter::ImportStaticMesh(AssetHandle handle, const AssetMetadata& metadata)
    {
        return LoadStaticMesh(Project::GetActiveAssetDirectory() / metadata.Filepath);
    }

    Ref<StaticMeshData> ModelImporter::LoadStaticMesh(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
            return nullptr;
        return ModelLoader::LoadStaticModel(filepath);
    }

    Ref<MeshData> ModelImporter::ImportMesh(AssetHandle handle, const AssetMetadata &metadata)
    {
        return LoadMesh(Project::GetActiveAssetDirectory() / metadata.Filepath);
    }

    Ref<MeshData> ModelImporter::LoadMesh(const std::filesystem::path& filepath)
    {
        if (!std::filesystem::exists(filepath))
            return nullptr;

        return ModelLoader::LoadModel(filepath);
    }

    Ref<SpriteSheet> SpriteSheetImporter::Import(AssetHandle handle, const AssetMetadata &metadata)
    {
        return Load(Project::GetActiveAssetDirectory() / metadata.Filepath);
    }

    Ref<SpriteSheet> SpriteSheetImporter::Load(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
            return nullptr;

        return SpriteSheet::Create(filepath);
    }

    Ref<Material> MaterialImporter::Import(AssetHandle handle, const AssetMetadata &metadata)
    {
        return Load(Project::GetActiveAssetDirectory() / metadata.Filepath);
    }

    Ref<Material> MaterialImporter::Load(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
            return nullptr;

        Ref<Material> material = Material::Create(filepath.stem().string());
        MaterialSerializer::Deserialize(filepath, material);
        return material;
    }
}
