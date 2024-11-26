// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef ASSET_IMPORTER_H
#define ASSET_IMPORTER_H

#include "AssetMetadata.h"
#include "Origin/Audio/FmodSound.h"
#include "Origin/Scene/SpriteSheet.h"

#include "Origin/Renderer/Font.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/Material.h"
#include "Origin/Renderer/Model.h"

#include <queue>
#include <future>

namespace origin
{
    class Scene;
    using AssetImportFunction = std::function<Ref<Asset>(AssetHandle, const AssetMetadata &)>;

    class AssetTaskBase
    {
    public:
        virtual ~AssetTaskBase() = default;
        virtual bool Execute() = 0;
    };

    template<typename FutureT>
    class AssetTask : public AssetTaskBase
    {
    public:
        Ref<Asset> *Value;
        std::future<FutureT> Future;

        template<typename Func, typename... Args>
        AssetTask(Ref<Asset> *asset, Func &&func, Args&&... args)
            : Value(asset)
        {
            Future = std::async(std::launch::async, std::forward<Func>(func), std::forward<Args>(args)...);
        }

        // move constructor to allow transfer of ownership of Future
        AssetTask(AssetTask &&other) noexcept
            : Value(std::move(other.Value)), Future(std::move(other.Future))
        {}

        // this should be executed in main thread
        virtual bool Execute() override
        {
            if (Future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
            {
                void *data = Future.get();
                if (static_cast<FontData *>(data))
                {
                    *Value = Font::Create((FontData *)data);
                }
                return true;
            }
            return false;
        }

        // move assignment operator
        AssetTask &operator=(AssetTask &&other) noexcept
        {
            if (this != &other)
            {
                Value = std::move(other.Value);
                Future = std::move(other.Future);
            }
            return *this;
        }

        // disable copy constructor and assignment operator
        AssetTask(const AssetTask &) = delete;
        AssetTask &operator=(const AssetTask &) = delete;
    };

    struct AssetTaskWorker
    {
        template<typename T>
        void AddTask(AssetTask<T> &task)
        {
            TaskQueue.push(std::make_unique<AssetTask<T>>(std::move(task)));
        }

        // update this on main thread
        void Update(Timestep ts);

        std::queue<Scope<AssetTaskBase>> TaskQueue;
        float Timer = 0.0f;
    };

    class AssetImporter
    {
    public:
        static Ref<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
        static void SyncToMainThread(Timestep ts);
    };

    class FontImporter
    {
    public:
        static Ref<Font> Import(AssetHandle handle, AssetMetadata metadata);
        static void LoadAsync(Ref<Asset> *font, const std::filesystem::path &filepath);

    private:
        friend class AssetImporter;
    };

    class AudioImporter
    {
    public:
        static Ref<FmodSound> Import(AssetHandle handle, AssetMetadata metadata);
        static Ref<FmodSound> LoadAudioSource(const std::filesystem::path &filepath);
        static Ref<FmodSound> LoadStreamingSource(const std::filesystem::path &filepath);
    };

    class SceneImporter
    {
    public:
        static Ref<Scene> Import(AssetHandle handle, const AssetMetadata& metadata);
        static Ref<Scene> LoadScene(const std::filesystem::path &filepath);
        static AssetHandle OpenScene(const std::filesystem::path &filepath);
        static void SaveScene(Ref<Scene> scene, const std::filesystem::path &path);
    };

    class TextureImporter
    {
    public:
        static Ref<Texture2D> ImportTexture2D(AssetHandle handle, const AssetMetadata& metadata);
        static Ref<Texture2D> LoadTexture2D(const std::filesystem::path &path);
    };

    class ModelImporter
    {
    public:
        static Ref<Model> Import(AssetHandle handle, const AssetMetadata &metadata);
        static Ref<Model> Load(const std::filesystem::path& filepath);
    };

    class SpriteSheetImporter
    {
    public:
        static Ref<SpriteSheet> Import(AssetHandle handle, const AssetMetadata &metadata);
        static Ref<SpriteSheet> Load(const std::filesystem::path &filepath);
    };

    class MaterialImporter
    {
    public:
        static Ref<Material> Import(AssetHandle handle, const AssetMetadata &metadata);
        static Ref<Material> Load(const std::filesystem::path &filepath);
    };
}

#endif