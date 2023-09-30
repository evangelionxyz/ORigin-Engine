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
			particle.Position += particle.Velocity * deltaTime;
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

			float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(particle.Position))
				* glm::rotate(glm::mat4(1.0f), particle.Rotation.x, glm::vec3(1.0f, 0.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), particle.Rotation.y, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::rotate(glm::mat4(1.0f), particle.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(particle.Scale * size));

			Renderer2D::DrawQuad(transform, color, m_EntityID);
		}

		Renderer2D::EndScene();
	}

	void ParticleSystem::Emit(const ParticleProps& props)
	{
		Particle& particle = m_ParticlePool[m_PoolIndex];
		particle.Active = true;
		particle.Position = props.Position;
		particle.Scale = props.Scale;
		particle.Rotation = props.Rotation * Random::Float() * 2.0f * glm::pi<float>();

		// Velocity
		particle.Velocity = props.Velocity;
		particle.Velocity.x += props.VelocityVariation.x * (Random::Float() - 0.5f);
		particle.Velocity.y += props.VelocityVariation.y * (Random::Float() - 0.5f);

		// Color
		particle.ColorBegin = props.ColorBegin;
		particle.ColorEnd = props.ColorEnd;

		particle.LifeTime = props.LifeTime;
		particle.LifeRemaining = props.LifeTime;
		particle.SizeBegin = props.SizeBegin + props.SizeVariation * (Random::Float() - 0.5f);
		particle.SizeEnd = props.SizeEnd;

		m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
	}

	void ParticleSystem::Emit(ParticleComponent& component, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation, int entityID)
	{
		m_EntityID = entityID;

		ParticleProps props;

		props.Position = position;
		props.Scale = scale;
		props.Rotation = rotation;

		props.Velocity = component.Velocity;

		props.VelocityVariation = component.VelocityVariation;

		props.SizeBegin = component.SizeBegin;
		props.SizeEnd = component.SizeEnd;

		props.SizeVariation = component.SizeVariation;

		props.ColorBegin = component.ColorBegin;
		props.ColorEnd = component.ColorEnd;

		props.LifeTime = component.LifeTime;

		Emit(props);
	}

	std::shared_ptr<ParticleSystem> ParticleSystem::Create()
	{
		return std::make_shared<ParticleSystem>();
	}
}