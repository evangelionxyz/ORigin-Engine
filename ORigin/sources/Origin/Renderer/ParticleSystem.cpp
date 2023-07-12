// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "ParticleSystem.h"

#include "Renderer2D.h"
#include "Origin\Core\Random.h"

#include <glm\gtc\constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\compatibility.hpp>

namespace origin {

	ParticleSystem::ParticleSystem()
	{
		m_ParticlePool.resize(m_PoolIndex + 1);
	}

	void ParticleSystem::OnUpdate(float deltaTime)
	{
		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			if (particle.LifeRemaining <= 0.0f)
			{
				particle.Active = false;
				continue;
			}

			particle.LifeRemaining -= deltaTime;
			particle.Position += glm::vec3(particle.Velocity, 0.0f) * deltaTime;

			particle.Rotation += 0.01f * deltaTime;
		}
	}

	void ParticleSystem::OnRender()
	{
		Renderer2D::BeginScene();

		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			// Fade away particles
			float life = particle.LifeRemaining / particle.LifeTime;
			glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
			//color.a = color.a * life;

			float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(particle.Position))
				* glm::rotate(glm::mat4(1.0f), particle.Rotation, glm::vec3(0.0f, 0.0f, 1.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(size, size, 1.0f));

			Renderer2D::DrawQuad(transform, color, m_EntityID);
		}

		Renderer2D::EndScene();
	}

	void ParticleSystem::Emit(const ParticleProps& particleProps)
	{
		Particle& particle = m_ParticlePool[m_PoolIndex];
		particle.Active = true;
		particle.Position = particleProps.Position;
		particle.Rotation = Random::Float() * 2.0f * glm::pi<float>();

		// Velocity
		particle.Velocity = particleProps.Velocity;
		particle.Velocity.x += particleProps.VelocityVariation.x * (Random::Float() - 0.5f);
		particle.Velocity.y += particleProps.VelocityVariation.y * (Random::Float() - 0.5f);

		// Color
		particle.ColorBegin = particleProps.ColorBegin;
		particle.ColorEnd = particleProps.ColorEnd;

		particle.LifeTime = particleProps.LifeTime;
		particle.LifeRemaining = particleProps.LifeTime;
		particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * (Random::Float() - 0.5f);
		particle.SizeEnd = particleProps.SizeEnd;

		m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
	}

	void ParticleSystem::Emit(Particle2DComponent& component, const glm::vec3& position, int entityID)
	{
		m_EntityID = entityID;

		ParticleProps particleProps;

		particleProps.Position = position;
		particleProps.Velocity = component.Velocity;
		particleProps.VelocityVariation = component.VelocityVariation;
		particleProps.SizeBegin = component.SizeBegin;
		particleProps.SizeEnd = component.SizeEnd;
		particleProps.SizeVariation = component.SizeVariation;
		particleProps.ColorBegin = component.ColorBegin;
		particleProps.ColorEnd = component.ColorEnd;
		particleProps.LifeTime = component.LifeTime;

		Emit(particleProps);
	}

	std::shared_ptr<ParticleSystem> ParticleSystem::Create()
	{
		return std::make_shared<ParticleSystem>();
	}
}