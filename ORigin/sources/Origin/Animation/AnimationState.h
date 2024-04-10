// Copyright (c) Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Animation.h"
#include <unordered_map>

namespace origin {
	
	template<typename T>
	class AnimationState
	{
	public:
		AnimationState() = default;

		void AnimationState<T>::AddState(std::string state)
		{
			if (StateExists(state))
			{
				OGN_CORE_ERROR("Animation State '{}' already Exists", state);
				return;
			}

			if (m_StateStorage.empty())
				m_DefaultState = state;

			m_CurrentState = state;
			m_StateStorage.emplace_back(m_CurrentState);
		}

		void AnimationState<T>::AddAnimation(const std::shared_ptr<T> &anim)
		{
			OGN_CORE_ASSERT(!AnimationExists(m_CurrentState), "Animation already exist");
			m_Animations[m_CurrentState] = std::move(anim);
		}

		void AnimationState<T>::RemoveState(std::string state)
		{
			OGN_CORE_ASSERT(AnimationExists(state), "Animation doesn't exist");
			m_Animations.erase(state);
			auto it = std::find(m_StateStorage.begin(), m_StateStorage.end(), state);
			m_StateStorage.erase(it);
		}

		void AnimationState<T>::SetDefaultState(std::string state)
		{
			m_DefaultState = state;
		}

		void AnimationState<T>::SetActiveState(std::string state)
		{
			m_CurrentState = state;
		}

		void AnimationState<T>::Stop()
		{
			OGN_CORE_ASSERT(AnimationExists(m_CurrentState), "Animation doesn't exist");
			m_Animations.at(m_CurrentState).Reset();
		}

		void AnimationState<T>::OnUpdateEditor(float dt)
		{
			OGN_CORE_ASSERT(AnimationExists(m_CurrentState), "Animation doesn't exist");
			m_Animations.at(m_CurrentState)->OnUpdateEditor(dt);
		}

		void AnimationState<T>::OnUpdateRuntime(float dt)
		{
			OGN_CORE_ASSERT(AnimationExists(m_CurrentState), "Animation doesn't exist");
			m_Animations.at(m_CurrentState)->OnUpdateRuntime(dt);
		}

		void AnimationState<T>::SetLooping(std::string state, bool looping)
		{
			OGN_CORE_ASSERT(AnimationExists(state), "Animation doesn't exist");
			m_Animations.at(state).Looping = looping;
		}

		bool AnimationState<T>::IsLooping(std::string state)
		{
			OGN_CORE_ASSERT(AnimationExists(state), "Animation doesn't exist");
			return m_Animations.at(state).Looping;
		}

		std::shared_ptr<T> &AnimationState<T>::GetAnimation()
		{
			return m_Animations.at(m_CurrentState);
		}

		bool AnimationState<T>::AnimationExists(std::string state)
		{
			return m_Animations.find(state) != m_Animations.end();
		}

		bool AnimationState<T>::StateExists(std::string stateName)
		{
			auto it = std::find(m_StateStorage.begin(), m_StateStorage.end(), stateName);
			return it != m_StateStorage.end();
		}

		bool AnimationState<T>::HasAnimations() const
		{ 
			return !m_Animations.empty();
		}

		bool AnimationState<T>::HasStates() const
		{ 
			return m_StateStorage.size() > 0;
		}

		bool AnimationState<T>::Preview = false;

		std::string AnimationState<T>::GetCurrentState() const
		{ 
			return m_CurrentState;
		}

		std::string AnimationState<T>::GetDefaultState() const
		{ 
			return m_DefaultState;
		}

		std::unordered_map<std::string, std::shared_ptr<T>> AnimationState<T>::GetAnimationState() const
		{
			return m_Animations;
		}

		const std::vector<std::string>&AnimationState<T>::GetStateStorage() const
		{
			return m_StateStorage;
		}

	private:
		std::string AnimationState<T>::m_CurrentState;
		std::string AnimationState<T>::m_DefaultState;
		std::unordered_map<std::string, std::shared_ptr<T>> AnimationState<T>::m_Animations;
		std::vector<std::string> AnimationState<T>::m_StateStorage;
	};

	

	

}

