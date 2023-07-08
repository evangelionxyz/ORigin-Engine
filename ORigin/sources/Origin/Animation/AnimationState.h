// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Animation.h"
#include <unordered_map>

namespace origin {

	class AnimationState
	{
	public:
		AnimationState() = default;

		void SetActiveState(std::string state);

		void AddState(std::string state);
		void AddAnim(const std::shared_ptr<Animation>& anim);

		void RemoveState(const std::string& state);
		void Stop();

		void Update(float deltaTime);

		void SetLooping(bool looping);
		bool IsLooping();
		bool HasState() { return m_StateStorage.size() > 0; }
		bool HasAnimation() { return m_Animations.size() > 0; }

		bool Exists(const std::string& state);

		const std::shared_ptr<Animation>& GetAnimation();
		const std::string GetCurrentStateString() { return m_CurrentState; }
		const std::unordered_map<std::string, std::shared_ptr<Animation>> GetAnimationState() { return m_Animations; }
		const std::vector<std::string>& GetStateStorage() { return m_StateStorage; }

	private:
		std::string m_CurrentState = "";
		std::unordered_map<std::string, std::shared_ptr<Animation>> m_Animations;
		std::vector<std::string> m_StateStorage;
	};
}

