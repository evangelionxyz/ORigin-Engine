// Copyright (c) Evangelion Manuhutu | ORigin Engine
#include "pch.h"

#include "Asset.h"
#include "AssetImporter.h"
#include "Origin/Core/Application.h"
#include "Origin/Project/Project.h"
#include "Origin/Serializer/SceneSerializer.h"
#include "Origin/Serializer/SpriteSheetSerializer.h"
#include "Origin/Utils/PlatformUtils.h"
#include "Origin/Renderer/Materials/Material.hpp"
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
        { AssetType::Mesh, ModelImporter::Import },
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
        font->Handle = handle;
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
    
    Ref<FmodSound> AudioImporter::Import(AssetHandle handle, AssetMetadata metadata)
    {
        Ref<FmodSound> sound = LoadAudioSource(Project::GetActiveAssetDirectory() / metadata.Filepath);
        sound->Handle = handle;
        return sound;
    }

    Ref<FmodSound> AudioImporter::LoadAudioSource(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
            return nullptr;

        Ref<FmodSound> fmod_sound = FmodSound::Create("Audio", filepath.generic_string());
        return fmod_sound;
    }

    Ref<FmodSound> AudioImporter::LoadStreamingSource(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
            return nullptr;

        Ref<FmodSound> fmod_sound = FmodSound::CreateStream("Audio", filepath.generic_string());
        return fmod_sound;
    }

    Ref<Scene> SceneImporter::Import(AssetHandle handle, const AssetMetadata& metadata)
    {
        Ref<Scene> scene = LoadScene(Project::GetActiveAssetDirectory() / metadata.Filepath);
        scene->Handle = handle;
        return scene;
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
        PUSH_CONSOLE_INFO("Scene saved! {}", filepath.generic_string());
    }

    Ref<Texture2D> TextureImporter::ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata)
    {
        Ref<Texture2D> texture = LoadTexture2D(Project::GetActiveAssetDirectory() / metadata.Filepath);
        texture->Handle = handle;
        return texture;
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

    Ref<Model> ModelImporter::Import(AssetHandle handle, const AssetMetadata& metadata)
    {
        return Load(Project::GetActiveAssetDirectory() / metadata.Filepath);
    }

    Ref<Model> ModelImporter::Load(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
            return nullptr;

        return Model::Create(filepath.generic_string());
    }

    Ref<SpriteSheet> SpriteSheetImporter::Import(AssetHandle handle, const AssetMetadata &metadata)
    {
        auto sprite_sheet = Load(Project::GetActiveAssetDirectory() / metadata.Filepath);
        if (sprite_sheet)
            sprite_sheet->Handle = handle;
        return sprite_sheet;
    }

    Ref<SpriteSheet> SpriteSheetImporter::Load(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
        {
            return nullptr;
        }
  
        Ref<SpriteSheet> sprite_sheet = SpriteSheet::Create();
        SpriteSheetSerializer::Deserialize(filepath, sprite_sheet);

        return sprite_sheet;
    }

    Ref<Material> MaterialImporter::Import(AssetHandle handle, const AssetMetadata &metadata)
    {
        Ref<Material> material = Load(Project::GetActiveAssetDirectory() / metadata.Filepath);
        //material->Handle = handle;
        return nullptr;
    }

    Ref<Material> MaterialImporter::Load(const std::filesystem::path &filepath)
    {
        if (!std::filesystem::exists(filepath))
            return nullptr;
        return nullptr;
    }
}
