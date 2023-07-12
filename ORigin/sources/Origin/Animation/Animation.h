// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once

#include "Origin\Renderer\Texture.h"
#include <vector>

namespace origin {

	struct AnimationFrame
	{
		std::shared_ptr<Texture2D> Texture;
		float FrameTime;

		AnimationFrame() = default;
		AnimationFrame(const std::shared_ptr<Texture2D>& texture, float frameTime)
			: Texture(texture), FrameTime(frameTime)
		{}
	};

	class Animation
	{
	public:
		Animation() = default;
		void AddFrame(const std::shared_ptr<Texture2D>& sprite, float frameTime);

		void Reset();
		void SetLooping(bool looping);
		void Update(float deltaTime);
		void Render(const glm::mat4& transform);

		void Delete();

		bool HasFrame() { return m_AnimationFrames.size() > 0; };
		bool IsLooping() { return m_Looping; }

		std::shared_ptr<Texture2D>& GetCurrentSprite();
		int GetFrameIndex() { return m_CurrentFrameIndex; }
		int GetFrameTotal() { return static_cast<int>(m_AnimationFrames.size()); }
		void SetFrameTime(float frameTime);

		static std::shared_ptr<Animation> Create();
		const std::vector<AnimationFrame> GetAnimationFrames() { return m_AnimationFrames; }

	private:
		std::vector<AnimationFrame> m_AnimationFrames;

		int m_CurrentFrameIndex = 0;
		float m_ElapsedFrameTime = 0.0f;
		bool m_Looping;
	};
}
