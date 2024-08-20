// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef ANIMATION_STATE_H
#define ANIMATION_STATE_H

#include "Animation.h"
#include <unordered_map>

namespace origin {
	
	template<typename T>
	class AnimationState
	{
	public:
		AnimationState() = default;

		void AddState(const std::string &state)
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

		void AddAnimation(std::shared_ptr<T> anim)
		{
			OGN_CORE_ASSERT(!AnimationExists(m_CurrentState), "Animation already exist");
			m_Animations[m_CurrentState] = anim;
		}

		void RemoveState(std::string state)
		{
			OGN_CORE_ASSERT(AnimationExists(state), "Animation doesn't exist");
			m_Animations.erase(state);

			auto it = std::find(m_StateStorage.begin(), m_StateStorage.end(), state);
			m_StateStorage.erase(it);

			m_CurrentState = m_StateStorage.size() > 0 ? m_StateStorage[0] : "";
		}

		void SetDefaultState(std::string state)
		{
			m_DefaultState = state;
		}

		void SetActiveState(std::string state)
		{
			m_CurrentState = state;
		}

		void Stop()
		{
			if (AnimationExists(m_CurrentState))
				m_Animations.at(m_CurrentState).Reset();
		}

		void OnUpdateEditor(float dt)
		{
			if(AnimationExists(m_CurrentState))
				m_Animations.at(m_CurrentState)->OnUpdateEditor(dt);
		}

		void OnUpdateRuntime(float dt)
		{
			if (AnimationExists(m_CurrentState))
				m_Animations.at(m_CurrentState)->OnUpdateRuntime(dt);
		}

		bool IsCurrentAnimationExists()
		{
			if (!HasAnimations())
				return false;

			return AnimationExists(m_CurrentState);
		}

		void SetLooping(std::string state, bool looping)
		{
			if (AnimationExists(m_CurrentState))
				m_Animations.at(state).Looping = looping;
		}

		bool IsLooping(std::string state)
		{
			if (AnimationExists(m_CurrentState))
				return m_Animations.at(state).Looping;
		}

		std::shared_ptr<T> &GetAnimation()
		{
			return m_Animations.at(m_CurrentState);
		}

		bool AnimationExists(std::string state)
		{
			return m_Animations.find(state) != m_Animations.end();
		}

		bool StateExists(std::string stateName)
		{
			auto it = std::find(m_StateStorage.begin(), m_StateStorage.end(), stateName);
			return it != m_StateStorage.end();
		}

		bool HasAnimations() const
		{ 
			return !m_Animations.empty();
		}

		bool HasStates() const
		{ 
			return m_StateStorage.size() > 0;
		}

		bool Preview = false;

		std::string GetCurrentState() const
		{ 
			return m_CurrentState;
		}

		std::string GetDefaultState() const
		{ 
			return m_DefaultState;
		}

		std::unordered_map<std::string, std::shared_ptr<T>> GetAnimationState() const
		{
			return m_Animations;
		}

		const std::vector<std::string>&GetStateStorage() const
		{
			return m_StateStorage;
		}

	private:
		std::string m_CurrentState;
		std::string m_DefaultState;
		std::unordered_map<std::string, std::shared_ptr<T>> m_Animations;
		std::vector<std::string> m_StateStorage;
	};
}

#endif