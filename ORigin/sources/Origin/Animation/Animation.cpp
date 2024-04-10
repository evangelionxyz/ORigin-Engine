// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Animation.h"

namespace origin {
	
	void SpriteAnimation::AddFrame(SpriteAnimationFrame frame)
	{
		AnimFrames.push_back(std::move(frame));
	}

	void SpriteAnimation::DeleteFrame(int index)
	{
		if (HasFrame())
		{
			AnimFrames.erase(AnimFrames.begin() + index);
			FrameIndex = index > 0 ? index - 1 : 0;
		}
	}

	void SpriteAnimation::OnUpdateEditor(Timestep ts)
	{
		if (Preview)
		{
			if (CurrentFrame < MaxFrame)
				CurrentFrame += ts * 60.0f * (Speed * 0.1f);

			if (CurrentFrame >= MaxFrame)
				CurrentFrame = Looping ? 0.0f: MaxFrame;
		}

		for (int i = 0; i < AnimFrames.size(); i++)
		{
			if (CurrentFrame >= AnimFrames[i].FrameBegin && CurrentFrame < AnimFrames[i].FrameEnd)
			{
				FrameIndex = i;
			}
		}
	}

	void SpriteAnimation::OnUpdateRuntime(Timestep ts)
	{
		if (!HasFrame())
			return;

		if (CurrentFrame < MaxFrame)
			CurrentFrame += ts * 60.0f * (Speed * 0.1f);

		if (CurrentFrame >= MaxFrame)
			CurrentFrame = Looping ? 0.0f : MaxFrame;

		for (int i = 0; i < AnimFrames.size(); i++)
		{
			if (CurrentFrame >= AnimFrames[i].FrameBegin && CurrentFrame < AnimFrames[i].FrameEnd)
			{
				FrameIndex = i;
			}
		}
	}

	void SpriteAnimation::Reset()
	{
		CurrentFrame = 0.0f;
	}

	SpriteAnimationFrame  &SpriteAnimation::GetCurrentFrame()
	{
		return AnimFrames[FrameIndex];
	}

	SpriteAnimationFrame  &SpriteAnimation::GetFrame(int index)
	{
		return AnimFrames[index];
	}
}
