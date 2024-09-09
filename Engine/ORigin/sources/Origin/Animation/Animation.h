// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef ANIMATION_H
#define ANIMATION_H

#include "Origin/Core/Base.h"

namespace origin
{
	enum class AnimationType
	{
		Sprite       = BIT(0),
		SpritesSheet = BIT(1),
		Transform    = BIT(2),
		Skeletal    = BIT(3)
	};

	static const char *AnimationTypeToString(AnimationType type)
	{
		switch (type)
		{
		case AnimationType::Sprite: return "Sprite";
		case AnimationType::SpritesSheet: return "SpritesSheet";
		case AnimationType::Transform: return "Transform";
		case AnimationType::Skeletal: return "Skeletal";
		default: return "Invalid Type";
		}
	}

	class Animation
	{
	public:
		virtual ~Animation() = default;

		bool Looping = false;
		bool Preview = false;

		virtual AnimationType GetType() const = 0;
	};
}

#endif