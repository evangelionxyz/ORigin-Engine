// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once

namespace origin {

	class AudioListener
	{
	public:
		AudioListener() = default;
		void Set(const glm::vec3& position, const glm::vec3& velocity, const glm::vec3& forward, const glm::vec3& up);
		
		const glm::vec3& GetVelocity() { return m_Velocity; }
		const glm::vec3& GetPosition() { return m_Position;}
	private:
		glm::vec3 m_Velocity = glm::vec3(0.0f);
		glm::vec3 m_Position = glm::vec3(0.0f);
	};
}


