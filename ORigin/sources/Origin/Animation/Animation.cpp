// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Animation.h"

#include "Origin\Renderer\Renderer2D.h"

namespace origin {

	void Animation::AddFrame(const std::shared_ptr<Texture2D>& sprite, float frameTime)
	{
		const AnimationFrame frame(sprite, frameTime);
		m_AnimationFrames.push_back(frame);
	}

	void Animation::Reset()
	{
		m_CurrentFrameIndex = 0;
	}

	void Animation::Update(float deltaTime)
	{
		if (!HasFrame())
			return;

		// Start from 0 second
		m_ElapsedFrameTime += deltaTime;

		// check each Frame's frameTime
		if (m_ElapsedFrameTime >= m_AnimationFrames[m_CurrentFrameIndex].FrameTime)
		{
			// Increment the frame index
			m_CurrentFrameIndex++;

			// Decide to Looping (0) or not (size - 1)
			if (m_CurrentFrameIndex >= m_AnimationFrames.size())
				m_CurrentFrameIndex = m_Looping ? 0 : m_AnimationFrames.size() - 1;

			// Reset the ElapsedFrameTime
			m_ElapsedFrameTime = 0.0f;
		}
		
	}

	void Animation::Render(const glm::mat4& transform)
	{
		GetCurrentSprite()->Bind();
		Renderer2D::DrawQuad(transform, GetCurrentSprite());
	}

	void Animation::Delete()
	{
		m_CurrentFrameIndex = 0;
		m_AnimationFrames.clear();
	}

	void Animation::SetLooping(bool looping)
	{
		m_Looping = looping;
	}

	std::shared_ptr<Texture2D> Animation::GetCurrentSprite()
	{
		return m_AnimationFrames.at(m_CurrentFrameIndex).Texture;
	}

	std::shared_ptr<Texture2D> Animation::GetSprites(int frame)
	{
		return m_AnimationFrames.at(frame).Texture;
	}

	void Animation::SetFrameTime(float frameTime)
	{
		for (auto& frame : m_AnimationFrames)
			frame.FrameTime = frameTime;
	}
	std::shared_ptr<Animation> Animation::Create()
	{
		return std::make_shared<Animation>();
	}
}
