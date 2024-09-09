// Copyright (c) 2022-present Evangelion Manuhutu | ORigin Engine

#ifndef AUDIO_LISTENER_H
#define AUDIO_LISTENER_H

#include <glm/glm.hpp>

namespace origin
{
    class AudioListener
    {
    public:
        AudioListener() = default;
        AudioListener(const glm::vec3 &position, const glm::vec3 &velocity, const glm::vec3 &forward, const glm::vec3 &up);

        void Set(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up);
        void SetEnable(bool enable);
    };
}

#endif