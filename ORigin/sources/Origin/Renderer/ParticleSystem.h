// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#pragma once
#include "Shader.h"

namespace origin {

	struct ParticleProps
	{
		glm::vec3 Position;
		glm::vec2 Velocity;
		glm::vec2 VelocityVariation;
		glm::vec4 ColorBegin;
		glm::vec4 ColorEnd;

		float SizeBegin;
		float SizeEnd;
		float SizeVariation;
		float LifeTime;
	};

	struct Particle2DComponent;
	class ParticleSystem
	{
	public:
		ParticleSystem();

		void OnUpdate(float deltaTime);
		void OnRender();

		void Emit(const ParticleProps& particleProps);
		void Emit(Particle2DComponent& component, const glm::vec3& position, int entityID = -1);

		static std::shared_ptr<ParticleSystem> Create();
	private:
		struct Particle
		{
			glm::vec3 Position;
			glm::vec2 Velocity;
			glm::vec4 ColorBegin, ColorEnd;
			float Rotation = 0.0f;
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
