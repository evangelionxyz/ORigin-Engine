// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "AnimationController.h"

namespace origin {

	template<typename T>
	AnimationController<T>::AnimationController(std::shared_ptr<AnimationState<T>> &state)
		: m_State(state)
	{
	}

	template<typename T>
	void AnimationController<T>::AddAnimationState(const std::shared_ptr<T> &anim, const std::string &state) const
	{
		m_State->AddState(state);
		m_State->AddAnimation(anim);
	}

	template<typename T>
	void AnimationController<T>::AddTransitionCondition(const std::string &name, bool condition)
	{
		m_Conditions[name] = condition;
	}

	template<typename T>
	std::shared_ptr<AnimationController<T>> AnimationController<T>::Create(std::shared_ptr<AnimationState<T>>& state)
	{
		return std::make_shared<AnimationController<T>>(state);
	}

}
