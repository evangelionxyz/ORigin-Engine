// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "AnimationState.h"

namespace origin {

	void AnimationState::AddState(std::string state)
	{
		OGN_CORE_ASSERT(!Exists(state), "Animation already exist");
		m_CurrentState = state;

		m_StateStorage.emplace_back(m_CurrentState);
	}

	void AnimationState::AddAnim(const std::shared_ptr<Animation>& anim)
	{
		OGN_CORE_ASSERT(!Exists(m_CurrentState), "Animation already exist");
		m_Animations[m_CurrentState] = anim;

		// storing state name into storage
		for (auto& it : m_Animations)
			m_StateStorage.emplace_back(it.first);
	}

	void AnimationState::RemoveState(const std::string& state)
	{
		OGN_CORE_ASSERT(Exists(state), "Animation doesn't exist");

		m_Animations.at(state)->Delete();
		m_Animations.erase(state);

		std::vector<std::string>::iterator it = std::find(m_StateStorage.begin(), m_StateStorage.end(), state);
		m_StateStorage.erase(it);
	}

	void AnimationState::SetActiveState(std::string state)
	{
		m_CurrentState = state;
	}

	void AnimationState::Stop()
	{
		OGN_CORE_ASSERT(Exists(m_CurrentState), "Animation doesn't exist");
		m_Animations.at(m_CurrentState)->Reset();
	}

	void AnimationState::Update(float deltaTime)
	{
		OGN_CORE_ASSERT(Exists(m_CurrentState), "Animation doesn't exist");
		m_Animations.at(m_CurrentState)->Update(deltaTime);
	}

	void AnimationState::SetLooping(bool looping)
	{
		OGN_CORE_ASSERT(Exists(m_CurrentState), "Animation doesn't exist");
		m_Animations.at(m_CurrentState)->SetLooping(looping);
	}

	bool AnimationState::IsLooping()
	{
		OGN_CORE_ASSERT(Exists(m_CurrentState), "Animation doesn't exist");
		return m_Animations.at(m_CurrentState)->IsLooping();
	}

	const std::shared_ptr<Animation>& AnimationState::GetAnimation()
	{
		if (m_Animations.at(m_CurrentState))
			return m_Animations.at(m_CurrentState);
		else
			return nullptr;
	}

	bool AnimationState::Exists(const std::string& state)
	{
		if (m_Animations.find(state) == m_Animations.end())
		{
			OGN_CORE_ERROR("Animation State '{}' not found", state);
			OGN_CORE_ASSERT(false);
			return false;
		}

		return true;
	}
}