// Copyright (c) Evangelion Manuhutu | ORigin Engine
#pragma once

#include "Origin\Renderer\Texture.h"
#include <glm\glm.hpp>
#include <vector>

namespace origin {

	struct AnimationFrame
	{
		AssetHandle Handle;
		int FrameBegin = 0;
		int FrameEnd = 4;
		AnimationFrame() = default;
		AnimationFrame(AssetHandle handle)
			: Handle(handle) {}
	};

	enum class AnimationType
	{
		None = -1,
		SpriteSheet = 0,
		Transform = 1
	};

	static const char* AnimationTypeToString(AnimationType type)
	{
		switch (type)
		{
			case AnimationType::None: return "Invalid";
			case AnimationType::SpriteSheet: return "Sprite";
			case AnimationType::Transform: return "Tr";
		}

		return "Invalid";
	}

	class Animation
	{
	public:
		Animation() = default;
		Animation(std::string name);
		
		void AddFrame(AnimationFrame frame);
		void AddFrame(AssetHandle handle);
		void DeleteFrame(int index);

		void OnUpdateEditor();
		void OnUpdateRuntime();
		bool HasFrame() const { return AnimFrames.empty() == false; }

		AssetHandle GetCurrentValue();
		AssetHandle GetValue(int frame);

		size_t GetTotalFrames() const { return AnimFrames.size(); }
		AnimationFrame& GetFrame(int index);
		const std::string GetName() { return m_Name; }

		static std::shared_ptr<Animation> Create(const std::string& name);
		std::vector<AnimationFrame> AnimFrames;
		int CurrentFrame = 0;
		int MaxFrame = 60;

		int FrameIndex = 0;

		bool Looping = false;
		bool Preview = false;

		void SetType(AnimationType type) { m_Type = type; }
		AnimationType GetType() { return m_Type; }

	private:
		AnimationType m_Type = AnimationType::None;
		std::string m_Name;
	};
}
