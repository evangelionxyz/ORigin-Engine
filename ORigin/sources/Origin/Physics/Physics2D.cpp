#include "pch.h"
#include "Physics2D.h"

#include "Origin\Scene\Entity.h"
#include "Origin\Scene\Components.h"

#include "box2d\box2d.h"
#include "box2d\b2_world.h"
#include "box2d\b2_body.h"
#include "box2d\b2_fixture.h"
#include "box2d\b2_polygon_shape.h"
#include "box2d\b2_circle_shape.h"

#include "Contact2DListener.h"

namespace origin {

	static b2BodyType Box2DBodyType(Rigidbody2DComponent::BodyType type)
	{
		switch (type)
		{
		case Rigidbody2DComponent::BodyType::Static: return b2_staticBody;
		case Rigidbody2DComponent::BodyType::Dynamic: return b2_dynamicBody;
		case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
		}

		OGN_CORE_ASSERT(false, "Unkown Body Type");
		return b2_staticBody;
	}

	Physics2D::Physics2D(Scene* scene)
		: m_SceneContext(scene)
	{
	}

	void Physics2D::Simulate(float deltaTime)
	{
		constexpr int32_t velocityIterations = 6;
		constexpr int32_t positionIterations = 2;

		m_World->Step(deltaTime, velocityIterations, positionIterations);

		// Retrieve transform from Box2D
		auto& view = m_SceneContext->m_Registry.view<Rigidbody2DComponent>();
		for (auto& e : view)
		{
			Entity entity = { e, m_SceneContext };
			auto& transform = entity.GetComponent<TransformComponent>();
			const auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			const auto body = static_cast<b2Body*>(rb2d.RuntimeBody);

			if (body)
			{
				const auto& position = body->GetPosition();
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				transform.Rotation.z = body->GetAngle();
			}
		}
	}

	void Physics2D::OnSimulationStart()
	{
		m_World = new b2World({ 0.0f, -9.81f });

		m_ContactListener = new Contact2DListener(m_SceneContext);
		m_World->SetContactListener(m_ContactListener);

		auto view = m_SceneContext->m_Registry.view<Rigidbody2DComponent>();
		for (entt::entity e : view)
		{
			Entity entity = { e, m_SceneContext };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;

			// set user data based on the entity
			bodyDef.userData.pointer = static_cast<uintptr_t>(e);

			bodyDef.type = Box2DBodyType(rb2d.Type);
			bodyDef.linearDamping = rb2d.LinearDamping;
			bodyDef.angularDamping = rb2d.AngularDamping;
			bodyDef.allowSleep = rb2d.AllowSleeping;
			bodyDef.awake = rb2d.Awake;
			bodyDef.bullet = rb2d.Bullet;
			bodyDef.enabled = rb2d.Enabled;

			bodyDef.linearVelocity.SetZero();

			// POSITION SETTINGS
			float xPos = transform.Translation.x;
			float yPos = transform.Translation.y;

			if (rb2d.FreezePositionX)
				xPos = bodyDef.position.x;
			if (rb2d.FreezePositionY)
				yPos = bodyDef.position.y;

			bodyDef.position.Set(xPos, yPos);

			// ROTATION SETTINGS
			bodyDef.angle = transform.Rotation.z;

			bodyDef.gravityScale = rb2d.GravityScale;

			b2Body* body = m_World->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);

			b2MassData massData;
			massData.center = b2Vec2(rb2d.MassCenter.x, rb2d.MassCenter.y);
			massData.I = rb2d.RotationalInertia;
			massData.mass = rb2d.Mass;
			body->SetMassData(&massData);

			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(
					bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y,
					b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f
				);

				bc2d.RuntimeBoxShape = &boxShape;

				b2FixtureDef fixtureDef;
				fixtureDef.filter.groupIndex = bc2d.Group;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;

				bc2d.RuntimeFixture = &fixtureDef;

				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = transform.Scale.x * cc2d.Radius;

				cc2d.RuntimeCircleShape = &circleShape;

				b2FixtureDef fixtureDef;
				fixtureDef.filter.groupIndex = cc2d.Group;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;

				cc2d.RuntimeFixture = &fixtureDef;

				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Physics2D::OnSimulationStop()
	{
		delete m_ContactListener;
		m_ContactListener = nullptr;

		m_World->SetContactListener(nullptr);

		delete m_World;
		m_World = nullptr;
	}

}