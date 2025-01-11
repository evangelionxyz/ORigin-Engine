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
		for (const auto &sprite : sprite_sheet->Sprites)
		{
			sr.BeginMap();
			sr.AddKeyValue("Rect", sprite.rect);
			sr.AddKeyValue("UV0", sprite.uv0);
			sr.AddKeyValue("UV1", sprite.uv1);
			sr.EndMap();
		}
		sr.EndSequence();
		
		sr.EndMap();
		
		sr.EndMap();

		sr.Serialize();

		return true;
	}

	bool SpriteSheetSerializer::Deserialize(const std::filesystem::path &filepath, Ref<SpriteSheet> &sprite_sheet)
	{
		YAML::Node s = Serializer::Deserialize(filepath);
		if (!s["SpriteSheet"])
			return false;

		if (YAML::Node data = s["SpriteSheet"])
		{
			AssetHandle texture_handle = data["Texture"].as<uint64_t>();
			sprite_sheet->SetTexture(texture_handle);
			sprite_sheet->SetGrid(data["Grid"].as<glm::vec2>());

			if (YAML::Node sprites = data["Sprites"])
			{
				for (auto sprite : sprites)
				{
					SpriteSheetData sp_data {};
					sp_data.texture_handle = texture_handle;
					sp_data.rect = sprite["Rect"].as<Rect>();
					sp_data.uv0 = sprite["UV0"].as<glm::vec2>();
					sp_data.uv1 = sprite["UV1"].as<glm::vec2>();
					sprite_sheet->Sprites.push_back(sp_data);
				}
			}
		}
		return true;
	}

}
