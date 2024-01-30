// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Animation.h"
#include <unordered_map>

namespace origin {

	class AnimationState
	{
	public:
		AnimationState() = default;

		void AddState(std::string state);
		void AddAnimation(Animation anim);

		void SetDefaultState(std::string state);
		void SetActiveState(std::string state);

		void RemoveState(std::string state);
		void Stop();

		void Update(float deltaTime);

		void SetLooping(std::string state, bool looping);
		bool IsLooping(std::string state);
		bool HasAnimations() const { return !m_Animations.empty(); }
		bool HasStates() const { return m_StateStorage.size() > 0; }
		bool Preview = false;

		bool AnimationExists(std::string state);
		bool StateExists(std::string stateName);

		std::string GetCurrentState() const { return m_CurrentState; }
		std::string GetDefaultState() const { return m_DefaultState; }

		Animation& GetAnimation();

		std::unordered_map<std::string, Animation> GetAnimationState() const { return m_Animations; }
		const std::vector<std::string>& GetStateStorage() const { return m_StateStorage; }

		int FrameMax = 60, CurrentFrame = 0;

	private:
		std::string m_CurrentState;
		std::string m_DefaultState;

		std::unordered_map<std::string, Animation> m_Animations;
		std::vector<std::string> m_StateStorage;
	};
}

