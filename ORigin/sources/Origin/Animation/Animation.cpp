// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Animation.h"
#include "Origin\Asset\AssetManager.h"

#include "Origin\Renderer\Renderer2D.h"

#pragma warning(disable : OGN_DISABLED_WARNINGS)

namespace origin {

	Animation::Animation(std::string name)
		: m_Name(std::move(name))
	{
	}

	void Animation::AddFrame(AssetHandle handle)
	{
		AnimationFrame frame(handle);
		AnimFrames.push_back(frame);
	}

	void Animation::AddFrame(AnimationFrame frame)
	{
		AnimFrames.push_back(frame);
	}

	void Animation::DeleteFrame(int index)
	{
		if (HasFrame())
		{
			AnimFrames.erase(AnimFrames.begin() + index);
			FrameIndex = index > 0 ? index - 1 : 0;
		}

	}

	void Animation::OnUpdateEditor()
	{
		if (Preview)
		{
			if (CurrentFrame < MaxFrame)
				CurrentFrame++;

			if (CurrentFrame >= MaxFrame)
				CurrentFrame = Looping ? 0 : MaxFrame;
		}

		for (int i = 0; i < AnimFrames.size(); i++)
		{
			if (CurrentFrame >= AnimFrames[i].FrameBegin && CurrentFrame < AnimFrames[i].FrameEnd)
			{
				FrameIndex = i;
			}
		}
	}

	void Animation::OnRuntimeUpdate()
	{
		if (!HasFrame())
			return;

		if (CurrentFrame < MaxFrame)
			CurrentFrame++;

		if (CurrentFrame >= MaxFrame)
			CurrentFrame = Looping ? 0 : MaxFrame;

		for (int i = 0; i < AnimFrames.size(); i++)
		{
			if (CurrentFrame >= AnimFrames[i].FrameBegin && CurrentFrame < AnimFrames[i].FrameEnd)
			{
				FrameIndex = i;
			}
		}
	}

	void Animation::Reset()
	{
		CurrentFrame = 0;
	}

	AssetHandle Animation::GetCurrentValue()
	{
		return AnimFrames[FrameIndex].Handle;
	}

	AssetHandle Animation::GetValue(int frame)
	{
		return AnimFrames.at(frame).Handle;
	}

	AnimationFrame& Animation::GetFrame(int index)
	{
		return AnimFrames[index];
	}

	std::shared_ptr<Animation> Animation::Create(const std::string& name)
	{
		return std::make_shared<Animation>(name);
	}
}
