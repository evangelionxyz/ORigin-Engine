
#include "AudioListener.h"

#include "AudioEngine.h"

namespace origin {

	void AudioListener::Set(const glm::vec3& p, const glm::vec3& v, const glm::vec3& f, const glm::vec3& up)
	{
		AudioEngine::SetListener(p, v, f, up);
	}
}