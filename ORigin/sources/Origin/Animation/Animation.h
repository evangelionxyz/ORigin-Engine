// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once

#include "Origin/Renderer/Texture.h"
#include "Origin/Core/Time.h"
#include "Origin/Core/Base.h"
#include <glm/glm.hpp>
#include <vector>

namespace origin
{
	enum class AnimationType
	{
		Sprite				= BIT(0),
		SpritesSheet	= BIT(1),
		Transform			= BIT(2)
	};

	static const char *AnimationTypeToString(AnimationType type)
	{
		switch (type)
		{
			case AnimationType::Sprite: return "Sprite";
			case AnimationType::SpritesSheet: return "SpritesSheet";
			case AnimationType::Transform: return "Transform";
		}
		return "Invalid";
	}

	struct SpriteAnimationFrame
	{
		int FrameBegin = 0;
		int FrameEnd = 4;

		glm::vec2 Min = glm::vec2(0.0f);
		glm::vec2 Max = glm::vec2(1.0f);

		AssetHandle Handle;
		SpriteAnimationFrame(AssetHandle handle)
			: Handle(handle)
		{
		}
	};

	class SpriteAnimation
	{
	public:
		SpriteAnimation() = default;

		void AddFrame(SpriteAnimationFrame frame);
		void DeleteFrame(int index);

		void OnUpdateEditor(Timestep ts);
		void OnUpdateRuntime(Timestep ts);
		bool HasFrame() const { return AnimFrames.empty() == false; }
		void Reset();
		size_t GetTotalFrames() const { return AnimFrames.size(); }
		SpriteAnimationFrame &GetCurrentFrame();
		SpriteAnimationFrame &GetFrame(int index);
		std::vector<SpriteAnimationFrame> AnimFrames;

		int MaxFrame = 60;
		int MinFrame = 0;
		int FrameIndex = 0;
		float CurrentFrame = 0.0f;
		float Speed = 100.0f;

		bool Looping = false;
		bool Preview = false;
	};
}
