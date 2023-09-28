// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "AnimationController.h"

namespace origin {

	AnimationController::AnimationController(std::shared_ptr<AnimationState>& state)
		: m_State(state)
	{
	}

	void AnimationController::AddAnimationState(std::shared_ptr<Animation>& anim, const std::string& state) const
	{
		m_State->AddState(state);
		m_State->AddAnim(anim);
	}

	void AnimationController::AddTransitionCondition(const std::string& name, bool condition)
	{
		m_Conditions[name] = condition;
	}

	std::shared_ptr<AnimationController> AnimationController::Create(std::shared_ptr<AnimationState>& state)
	{
		return std::make_shared<AnimationController>(state);
	}

}
