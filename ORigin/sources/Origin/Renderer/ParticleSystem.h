// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Shader.h"

#include <vector>

namespace origin {

	struct ParticleProps
	{
		glm::vec3 Position;
		glm::vec3 Rotation;
		glm::vec3 Scale;

		glm::vec3 Velocity;
		glm::vec3 VelocityVariation;
		glm::vec4 ColorBegin;
		glm::vec4 ColorEnd;

		float SizeBegin;
		float SizeEnd;
		float SizeVariation;
		float LifeTime;
	};

	class ParticleComponent;
	class ParticleSystem
	{
	public:
		ParticleSystem();

		void OnUpdate(float deltaTime);
		void OnRender();

		void Emit(const ParticleProps& particleProps);
		void Emit(ParticleComponent& component, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation, int entityID = -1);

		static std::shared_ptr<ParticleSystem> Create();

	private:

		struct Particle
		{
			glm::vec3 Position;
			glm::vec3 Rotation;
			glm::vec3 Scale;

			glm::vec3 Velocity;

			glm::vec4 ColorBegin, ColorEnd;
			float SizeBegin, SizeEnd;

			float LifeTime = 1.0f;
			float LifeRemaining = 0.0f;

			bool Active = false;
		};
		std::vector<Particle> m_ParticlePool;
		uint32_t m_PoolIndex = 9999;

		int m_EntityID = -1;
	};

}
