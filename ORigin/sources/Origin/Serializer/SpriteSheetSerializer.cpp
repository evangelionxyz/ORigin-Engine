// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Serializer.h"
#include "SpriteSheetSerializer.h"

#include <fstream>
#include <yaml-cpp\yaml.h>

namespace origin
{
	bool SpriteSheetSerializer::Serialize(const std::filesystem::path &filepath, const std::shared_ptr<SpriteSheet> &spriteSheet)
	{
		PROFILER_FUNCTION();

		YAML::Emitter out;
		out << YAML::BeginMap; // SpriteSheet
		out << YAML::Key << "SpriteSheet";
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Texture" << YAML::Value << spriteSheet->GetTextureHandle();
			out << YAML::Key << "Sprites" << YAML::Value << YAML::BeginSeq;
			for (auto &sprite : spriteSheet->Sprites)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Min" << sprite.Min;
				out << YAML::Key << "Max" << sprite.Max;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
		}
		out << YAML::EndMap; // !SpriteSheet

		std::ofstream outFile(filepath.string());
		outFile << out.c_str();

		return true;
	}

	bool SpriteSheetSerializer::Deserialize(const std::filesystem::path &filepath, std::shared_ptr<SpriteSheet> &spriteSheet)
	{
		PROFILER_FUNCTION();

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
				for (auto &s : sprites)
				{
					SpriteSheetData sprite {};
					sprite.Min = s["Min"].as<glm::vec2>();
					sprite.Max = s["Max"].as<glm::vec2>();
					spriteSheet->Sprites.push_back(sprite);
				}
			}
		}
		return true;
	}

}
