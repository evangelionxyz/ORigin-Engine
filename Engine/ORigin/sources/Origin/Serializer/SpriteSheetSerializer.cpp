// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Serializer.hpp"
#include "SpriteSheetSerializer.h"
#include "Origin/Profiler/Profiler.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace origin
{
	bool SpriteSheetSerializer::Serialize(const std::filesystem::path &filepath, const Ref<SpriteSheet> &sprite_sheet)
	{
		Serializer sr(filepath);

		sr.BeginMap();
		
		sr.BeginMap("SpriteSheet");
		sr.AddKeyValue("Texture", sprite_sheet->GetTextureHandle());
		sr.AddKeyValue("Grid", sprite_sheet->GetGrid());
		
		sr.BeginSequence("Sprites");
		for (auto &sprite : sprite_sheet->Sprites)
		{
			sr.BeginMap();
			sr.AddKeyValue("Rect", sprite.rect);
			sr.EndMap();
		}
		sr.EndSequence();
		
		sr.EndMap();
		
		sr.EndMap();

		sr.Serialize();

		return true;
	}

	bool SpriteSheetSerializer::Deserialize(const std::filesystem::path &filepath, Ref<SpriteSheet> &spriteSheet)
	{
		OGN_PROFILER_FUNCTION();

		std::ifstream stream(filepath);
		std::stringstream strStream;

		strStream << stream.rdbuf();

		YAML::Node s = YAML::Load(strStream.str());
		if (!s["SpriteSheet"])
			return false;

		if (YAML::Node data = s["SpriteSheet"])
		{
			AssetHandle textureHandle = data["Texture"].as<uint64_t>();
			spriteSheet->SetMainTexture(textureHandle);

			if (YAML::Node sprites = data["Sprites"])
			{
				for (auto sprite : sprites)
				{
					SpriteSheetData spriteSheetData {};
					spriteSheetData.rect = sprite["Rect"].as<Rect>();
					spriteSheet->Sprites.push_back(spriteSheetData);
				}
			}
		}
		return true;
	}

}
