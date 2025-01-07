// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef SPRITE_SHEET_SERIALIZER_H
#define SPRITE_SHEET_SERIALIZER_H

#include "Origin/Scene/SpriteSheet.h"
#include <filesystem>

namespace origin
{
	class SpriteSheetSerializer
	{
	public:
        static bool Serialize(const std::filesystem::path &filepath, const Ref<SpriteSheet> &spriteSheet);
		static bool Deserialize(const std::filesystem::path &filepath, Ref<SpriteSheet> &spriteSheet);
	};
}

#endif