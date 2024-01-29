// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once

#include "Origin\Renderer\Texture.h"
#include <glm\glm.hpp>
#include <vector>

namespace origin {

	struct AnimationFrame
	{
		AssetHandle Handle;
		float FrameTime;

		AnimationFrame() = default;
		AnimationFrame(AssetHandle handle, float frameTime)
			: Handle(handle), FrameTime(frameTime)
		{}
	};

	class Animation
	{
	public:
		Animation() = default;
		void AddFrame(AssetHandle handle, float frameTime);
		void DeleteFrame(int index);

		void Reset();
		void SetLooping(bool looping);
		void Update(float deltaTime);
		void Render(const glm::mat4& transform);

		void Delete();

		bool HasFrame() const { return m_AnimationFrames.empty() == false; }
		bool IsLooping() const { return m_Looping; }

		AssetHandle GetCurrentValue();
		AssetHandle GetValue(int frame);

		int GetFrameIndex() const { return m_CurrentFrameIndex; }
		size_t GetTotalFrames() const { return m_AnimationFrames.size(); }
		void SetAnimationFrameTime(int index, float frameTime);
		AnimationFrame& GetAnimationFrame(int index);

		static std::shared_ptr<Animation> Create();
		std::vector<AnimationFrame> GetAnimationFrames() { return m_AnimationFrames; }

	private:
		std::vector<AnimationFrame> m_AnimationFrames;

		int m_CurrentFrameIndex = 0;
		float m_ElapsedFrameTime = 0.0f;
		bool m_Looping = false;
	};
}
