// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Animation.h"
#include "Origin\Asset\AssetManager.h"

#include "Origin\Renderer\Renderer2D.h"

namespace origin {

	void Animation::AddFrame(AssetHandle handle, float frameTime)
	{
		AnimationFrame frame(handle, frameTime);
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

		m_ElapsedFrameTime += deltaTime;

		if (m_ElapsedFrameTime >= m_AnimationFrames[m_CurrentFrameIndex].FrameTime)
		{
			m_CurrentFrameIndex++;

			if (m_CurrentFrameIndex >= m_AnimationFrames.size())
				m_CurrentFrameIndex = m_Looping ? 0 : m_AnimationFrames.size() - 1;

			m_ElapsedFrameTime = 0.0f;
		}
		
	}

	void Animation::Render(const glm::mat4& transform)
	{
		if (AssetManager::GetAssetType(GetCurrentValue()) == AssetType::Texture2D)
		{
			Renderer2D::DrawQuad(transform, AssetManager::GetAsset<Texture2D>(GetCurrentValue()));
		}
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

	AssetHandle Animation::GetCurrentValue()
	{
		return m_AnimationFrames.at(m_CurrentFrameIndex).Handle;
	}

	AssetHandle Animation::GetValue(int frame)
	{
		return m_AnimationFrames.at(frame).Handle;
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
