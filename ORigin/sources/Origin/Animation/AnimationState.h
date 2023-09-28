// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Animation.h"
#include <unordered_map>

namespace origin {

	class AnimationState
	{
	public:
		AnimationState() = default;

		void AddState(std::string state);
		void AddAnim(std::shared_ptr<Animation> anim);

		void SetDefaultState(std::string state);
		void SetActiveState(std::string state);

		void RemoveState(std::string state);
		void Stop() const;

		void Update(float deltaTime) const;

		void SetLooping(bool looping) const;
		bool IsLooping() const;
		bool Preview = false;
		bool HasState() const { return m_StateStorage.size() > 0; }
		bool HasAnimation();

		bool AnimationExists(std::string state);
		bool StateExists(std::string stateName);

		const std::shared_ptr<Animation>& GetAnimation();
		std::string GetCurrentStateString() const { return m_CurrentState; }
		std::string GetDefaultState() const { return m_DefaultState; }
		std::unordered_map<std::string, std::shared_ptr<Animation>> GetAnimationState() { return m_Animations; }
		const std::vector<std::string>& GetStateStorage() { return m_StateStorage; }

	private:
		std::string m_CurrentState;
		std::string m_DefaultState;

		std::unordered_map<std::string, std::shared_ptr<Animation>> m_Animations;
		std::vector<std::string> m_StateStorage;
	};
}

