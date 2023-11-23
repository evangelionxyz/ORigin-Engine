#include "pch.h"
#include "AudioImporter.h"

#include "Origin\Project\Project.h"

namespace origin {

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
}

