// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin\Renderer\Texture.h"
#include <vector>

namespace origin {

	struct AnimationState
	{
		std::string State;
		std::string Trigger;

		AnimationState() = default;
		AnimationState(const std::string& state, const std::string& trigger)
			: State(state), Trigger(trigger)
		{}
	};

	struct AnimationFrame
	{
		std::shared_ptr<Texture2D> Sprite;
		float FrameTime;

		AnimationFrame() = default;
		AnimationFrame(std::shared_ptr<Texture2D>& sprite, float frameTime)
			: Sprite(sprite), FrameTime(frameTime)
		{}
	};

	class Animation
	{
	public:
		Animation() = default;

		void AddFrame(std::shared_ptr<Texture2D>& sprite, float frameTime);

		void Reset();
		void Update(float deltaTime);
		void Render();

		void SetLoop(bool looping);

		std::shared_ptr<Texture2D>& GetCurrentSprite();
		int GetFrameIndex() { return m_CurrentFrameIndex; }
		int GetFrameCount() { return m_AnimationFrames.size(); }
		void SetFrameTime(float frameTime);

	private:
		std::vector<AnimationFrame> m_AnimationFrames;

		int m_CurrentFrameIndex = 0.0f;
		float m_ElapsedFrameTime = 0.0f;
		bool m_Looping;
	};

}

