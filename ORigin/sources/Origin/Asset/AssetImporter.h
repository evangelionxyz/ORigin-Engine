// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once
#include "AssetMetadata.h"
#include "Origin/Audio/AudioSource.h"
#include "Origin/Scene/SpriteSheet.h"
#include "Origin/Renderer/Texture.h"
#include "Origin/Renderer/Model.h"

#include <future>

namespace origin
{
	class Scene;
	using AssetImportFunction = std::function<std::shared_ptr<Asset>(AssetHandle, const AssetMetadata &)>;

	class AssetTaskBase
	{
	public:
		virtual ~AssetTaskBase() { };
		virtual bool Execute() = 0;
	};

	template<typename FutureT>
	class AssetTask : public AssetTaskBase
	{
	public:
		std::shared_ptr<Asset> *Value;
		std::future<FutureT> Future;

		template<typename Func, typename... Args>
		AssetTask(std::shared_ptr<Asset> *asset, Func &&func, Args&&... args)
			: Value(asset)
		{
			Future = std::async(std::launch::async, std::forward<Func>(func), std::forward<Args>(args)...);
		}

		// Move constructor to allow transfer of ownership of Future
		AssetTask(AssetTask &&other) noexcept
			: Value(std::move(other.Value)), Future(std::move(other.Future))
		{}

		// this is should be executed in main thread
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

		// Move assignment operator
		AssetTask &operator=(AssetTask &&other) noexcept
		{
			if (this != &other)
			{
				Value = std::move(other.Value);
				Future = std::move(other.Future);
			}
			return *this;
		}

		// Disable copy constructor and assignment operator
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

		std::queue<std::unique_ptr<AssetTaskBase>> TaskQueue;
		float Timer = 0.0f;
	};

	class AssetImporter
	{
	public:
		static std::shared_ptr<Asset> ImportAsset(AssetHandle handle, const AssetMetadata& metadata);
		static void SyncToMainThread(Timestep ts);
	};

	class FontImporter
	{
	public:
		static std::shared_ptr<Font> Import(AssetHandle handle, AssetMetadata metadata);
		static void LoadAsync(std::shared_ptr<Asset> *font, const std::filesystem::path &filepath);

	private:
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
