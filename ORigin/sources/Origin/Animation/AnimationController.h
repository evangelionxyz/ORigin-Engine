// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Animation.h"
#include "AnimationState.h"

#include <unordered_map>

namespace origin {

	template<typename T>
	class AnimationController
	{
	private:
		float m_Speed;

	public:
		AnimationController(std::shared_ptr<AnimationState<T>>& state);

		void AddAnimationState(const std::shared_ptr<T> &anim, const std::string& state) const;
		void AddTransitionCondition(const std::string& name, bool condition);
		void SetSpeed(float speed) { m_Speed = speed; }

		std::shared_ptr<AnimationState<T>>& m_State;
		std::unordered_map<std::string, bool> m_Conditions;

		static std::shared_ptr<AnimationController> Create(std::shared_ptr<AnimationState<T>>& state);
	};
}

