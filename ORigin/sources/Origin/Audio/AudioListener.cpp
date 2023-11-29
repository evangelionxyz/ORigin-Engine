#include "pch.h"
#include "AudioListener.h"

#include "Audio.h"

namespace origin {

	void AudioListener::Set(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up)
	{
		// Set Global Listener
		m_Position = position;
		m_Velocity = velocity;
		
		FMOD_VECTOR pos = { -position.x, -position.y, -position.z };
		FMOD_VECTOR forwardDirection = { forward.x, forward.y, forward.z };
		FMOD_VECTOR upDirection = { up.x, up.y, up.z };
		FMOD_VECTOR vel = { velocity.x, velocity.y, velocity.z };

		AudioEngine::GetSystem()->set3DListenerAttributes(
			0,
			&pos,
			&vel,
			&forwardDirection,
			&upDirection
		);
	}
}