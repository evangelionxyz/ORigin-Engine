// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "AnimationState.h"

namespace origin {

	void AnimationState::AddState(std::string state)
	{
		if (!StateExists(state))
		{
			if (m_StateStorage.empty())
				m_DefaultState = state;

			m_CurrentState = state;
			m_StateStorage.emplace_back(m_CurrentState);

			return;
		}

		OGN_CORE_ERROR("Animation State '{}' already Exists", state);
	}

	void AnimationState::AddAnimation(Animation anim)
	{
		OGN_CORE_ASSERT(!AnimationExists(m_CurrentState), "Animation already exist");
		m_Animations[m_CurrentState] = std::move(anim);
	}

	void AnimationState::RemoveState(std::string state)
	{
		OGN_CORE_ASSERT(AnimationExists(state), "Animation doesn't exist");

		m_Animations.at(state).Delete();
		m_Animations.erase(state);

		auto it = std::find(m_StateStorage.begin(), m_StateStorage.end(), state);
		m_StateStorage.erase(it);
	}

	void AnimationState::SetDefaultState(std::string state)
	{
		m_DefaultState = state;
	}

	void AnimationState::SetActiveState(std::string state)
	{
		m_CurrentState = state;
	}

	void AnimationState::Stop()
	{
		OGN_CORE_ASSERT(AnimationExists(m_CurrentState), "Animation doesn't exist");
		m_Animations.at(m_CurrentState).Reset();
	}

	void AnimationState::Update(float deltaTime)
	{
		OGN_CORE_ASSERT(AnimationExists(m_CurrentState), "Animation doesn't exist");
		m_Animations.at(m_CurrentState).Update(deltaTime);
	}

	void AnimationState::SetLooping(bool looping)
	{
		OGN_CORE_ASSERT(AnimationExists(m_CurrentState), "Animation doesn't exist");
		m_Animations.at(m_CurrentState).SetLooping(looping);
	}

	bool AnimationState::IsLooping()
	{
		OGN_CORE_ASSERT(AnimationExists(m_CurrentState), "Animation doesn't exist");
		return m_Animations.at(m_CurrentState).IsLooping();
	}

	Animation& AnimationState::GetAnimation()
	{
		return m_Animations.at(m_CurrentState);
	}

	bool AnimationState::HasAnimation()
	{
		if (m_Animations.empty())
			return false;
		else if (!AnimationExists(m_CurrentState))
			return false;
		else
			return m_Animations.at(m_CurrentState).HasFrame();
	}

	bool AnimationState::AnimationExists(std::string state)
	{
		if (m_Animations.find(state) == m_Animations.end())
		{
			OGN_CORE_ERROR("Animation '{}' not found", state);
			return false;
		}

		return true;
	}

	bool AnimationState::StateExists(std::string stateName)
	{
		auto it = std::find(m_StateStorage.begin(), m_StateStorage.end(), stateName);
		if (it == m_StateStorage.end())
		{
			OGN_CORE_ERROR("Animation State '{}' not found", stateName);
			return false;
		}

		return true;
	}
}