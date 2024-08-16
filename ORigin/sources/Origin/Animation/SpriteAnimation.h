// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Animation.h"
#include "Origin/Asset/Asset.h"

#include "Origin/Core/Time.h"

#include <glm/glm.hpp>

namespace origin
{
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

	class SpriteAnimation : public Animation
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

		static AnimationType GetStaticType() { return AnimationType::Sprite; }
		AnimationType GetType() const override { return GetStaticType(); }
	};
}

