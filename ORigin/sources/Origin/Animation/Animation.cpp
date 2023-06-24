// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Animation.h"

#include "Origin\Renderer\Renderer2D.h"

namespace origin {

	void Animation::AddFrame(std::shared_ptr<Texture2D>& sprite, float frameTime)
	{
		AnimationFrame frame(sprite, frameTime);
		m_AnimationFrames.push_back(frame);
	}

	void Animation::Reset()
	{
		m_CurrentFrameIndex = 0;
	}

	void Animation::Update(float deltaTime)
	{
		m_ElapsedFrameTime += deltaTime;

		if (m_ElapsedFrameTime >= m_AnimationFrames[m_CurrentFrameIndex].FrameTime)
		{
			m_CurrentFrameIndex++;
			if (m_CurrentFrameIndex >= m_AnimationFrames.size())
				m_CurrentFrameIndex = m_Looping ? 0 : m_AnimationFrames.size() - 1;

			m_ElapsedFrameTime = 0.0f;
		}
	}

	void Animation::Render()
	{
		GetCurrentSprite()->Bind();
		glm::mat4 transform = glm::mat4(1.0f);
		Renderer2D::DrawQuad(transform, GetCurrentSprite());
	}

	void Animation::SetLoop(bool looping)
	{
		m_Looping = looping;
	}

	std::shared_ptr<Texture2D>& Animation::GetCurrentSprite()
	{
		return m_AnimationFrames.at(m_CurrentFrameIndex).Sprite;
	}
	void Animation::SetFrameTime(float frameTime)
	{
		for (auto& frame : m_AnimationFrames)
			frame.FrameTime = frameTime;
	}
}
