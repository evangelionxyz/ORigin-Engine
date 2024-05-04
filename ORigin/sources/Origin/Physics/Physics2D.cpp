#include "pch.h"
#include "Physics2D.h"

#include "Origin\Scene\Entity.h"
#include "Origin\Scene\Components.h"

#include "box2d\box2d.h"
#include "box2d\b2_world.h"
#include "box2d\b2_fixture.h"
#include "box2d\b2_polygon_shape.h"
#include "box2d\b2_circle_shape.h"
#include "box2d\b2_joint.h"

#include "Contact2DListener.h"

#include <glm/glm.hpp>

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
		: m_Context(scene)
	{
		OGN_PROFILER_PHYSICS();
	}

	void Physics2D::CreateRevoluteJoint(RevoluteJoint2DComponent rjc, b2Body* body, b2Vec2 anchorPoint)
	{
		OGN_PROFILER_PHYSICS();

		b2RevoluteJointDef jointDef;

		Entity connectedEntity = m_Context->GetEntityWithUUID(rjc.ConnectedBodyID);
		auto& rb2d = connectedEntity.GetComponent<Rigidbody2DComponent>();
		b2Body* connectedBody = static_cast<b2Body*>(rb2d.RuntimeBody);

		OGN_CORE_ASSERT(connectedBody, "Connected Body is Invalid");

		jointDef.Initialize(body, connectedBody, anchorPoint);

		jointDef.lowerAngle = glm::radians(rjc.LowerAngle);
		jointDef.upperAngle = glm::radians(rjc.UpperAngle);

		jointDef.enableLimit = rjc.EnableLimit;
		jointDef.maxMotorTorque = rjc.MaxMotorTorque;
		jointDef.motorSpeed = rjc.MotorSpeed;
		jointDef.enableMotor = rjc.EnableMotor;

		rjc.Joint = (b2RevoluteJoint*)m_World->CreateJoint(&jointDef);
	}

	void Physics2D::CreateBoxCollider(BoxCollider2DComponent bc2d, b2Body* body, b2Vec2 boxSize)
	{
		OGN_PROFILER_PHYSICS();

		b2PolygonShape boxShape;
		boxShape.SetAsBox(boxSize.x, boxSize.y, b2Vec2(bc2d.Offset.x, bc2d.Offset.y), 0.0f);
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

	void Physics2D::CreateCircleCollider(CircleCollider2DComponent cc2d, b2Body* body, float radius)
	{
		OGN_PROFILER_PHYSICS();

		b2CircleShape circleShape;
		circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
		circleShape.m_radius = radius;
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

	void Physics2D::Simulate(float deltaTime)
	{
		OGN_PROFILER_PHYSICS();

		constexpr int32_t velocityIterations = 6;
		constexpr int32_t positionIterations = 2;

		m_World->Step(deltaTime, velocityIterations, positionIterations);

		// Retrieve transform from Box2D
		auto rb2dView = m_Context->m_Registry.view<TransformComponent, Rigidbody2DComponent>();
		for (entt::entity e : rb2dView)
		{
			Entity entity{ e, m_Context };
			auto& [tc, rb2d] = rb2dView.get<TransformComponent, Rigidbody2DComponent>(e);
			auto body = static_cast<b2Body*>(rb2d.RuntimeBody);

			if (body)
			{
				auto& position = body->GetPosition();
				tc.WorldTranslation.x = position.x;
				tc.WorldTranslation.y = position.y;
				tc.WorldRotation.z = body->GetAngle();

				tc.Translation = tc.WorldTranslation;
				tc.Rotation = tc.WorldRotation;
			}
		}

		auto view = m_Context->m_Registry.view<IDComponent, TransformComponent>();
		for (entt::entity e : view)
		{
			auto &[idc, tc] = view.get<IDComponent, TransformComponent>(e);
			Entity entity { e, m_Context };

			if (!entity.HasComponent<Rigidbody2DComponent>() && idc.Parent != 0)
			{
				auto &parentTC = m_Context->GetEntityWithUUID(idc.Parent).GetComponent<TransformComponent>();
				glm::vec3 rotatedLocalPos = glm::rotate(glm::quat(parentTC.WorldRotation), tc.Translation);
				tc.WorldTranslation = rotatedLocalPos + parentTC.WorldTranslation;
				tc.WorldRotation = parentTC.WorldRotation + tc.Rotation;
				tc.WorldScale = tc.Scale * parentTC.WorldScale;
				
			}
		}
	}

	void Physics2D::OnSimulationStart()
	{
		OGN_PROFILER_PHYSICS();

		m_World = new b2World({ 0.0f, -9.81f });

		m_ContactListener = new Contact2DListener(m_Context);
		m_World->SetContactListener(m_ContactListener);

		auto view = m_Context->m_Registry.view<TransformComponent, Rigidbody2DComponent>();
		for (entt::entity e : view)
		{
			Entity entity = { e, m_Context };
			auto& [tc, rb2d] = view.get<TransformComponent, Rigidbody2DComponent>(e);

			b2BodyDef bodyDef;
			bodyDef.userData.pointer = static_cast<uintptr_t>(e);
			bodyDef.type = Box2DBodyType(rb2d.Type);
			bodyDef.linearDamping = rb2d.LinearDamping;
			bodyDef.angularDamping = rb2d.AngularDamping;
			bodyDef.allowSleep = rb2d.AllowSleeping;
			bodyDef.awake = rb2d.Awake;
			bodyDef.bullet = rb2d.Bullet;
			bodyDef.enabled = rb2d.Enabled;
			bodyDef.linearVelocity.SetZero();

			float xPos = tc.WorldTranslation.x;
			float yPos = tc.WorldTranslation.y;

			if (rb2d.FreezePositionX)
				xPos = bodyDef.position.x;
			if (rb2d.FreezePositionY)
				yPos = bodyDef.position.y;

			bodyDef.position.Set(xPos, yPos);
			bodyDef.angle = tc.WorldRotation.z;
			bodyDef.gravityScale = rb2d.GravityScale;

			b2Body* body = m_World->CreateBody(&bodyDef);

			rb2d.RuntimeBody = body;

			body->SetFixedRotation(rb2d.FixedRotation);

			b2MassData massData;
			massData.center = b2Vec2(rb2d.MassCenter.x, rb2d.MassCenter.y);
			massData.I = rb2d.RotationalInertia;
			massData.mass = rb2d.Mass;
			body->SetMassData(&massData);

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
				CreateBoxCollider(bc2d, body, b2Vec2(bc2d.Size.x * tc.WorldScale.x, bc2d.Size.y * tc.WorldScale.y));
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
				float radius = tc.WorldScale.x * cc2d.Radius;
				CreateCircleCollider(cc2d, body, radius);
			}
		}

		for (entt::entity e : view)
		{
			Entity entity = { e, m_Context };
			auto& [tc, rb2d] = view.get<TransformComponent, Rigidbody2DComponent>(e);
			b2Body* body = static_cast<b2Body*>(rb2d.RuntimeBody);

			if(entity.HasComponent<RevoluteJoint2DComponent>())
			{
				auto& rjc = entity.GetComponent<RevoluteJoint2DComponent>();
				b2Vec2 pos = body->GetPosition();
				b2Vec2 anchorPoint = b2Vec2(pos.x + rjc.AnchorPoint.x, pos.y + rjc.AnchorPoint.y);
				if (rjc.ConnectedBodyID != 0)
					CreateRevoluteJoint(rjc, body, anchorPoint);
			}
		}
	}

	void Physics2D::OnSimulationStop()
	{
		OGN_PROFILER_PHYSICS();

		auto view = m_Context->m_Registry.view<Rigidbody2DComponent, RevoluteJoint2DComponent>();
		for (entt::entity e : view)
		{
			auto& rb2d = view.get<Rigidbody2DComponent>(e);
			b2Body* body = static_cast<b2Body*>(rb2d.RuntimeBody);
			if (body)
			{
				m_World->DestroyBody(body);
				body = nullptr;
			}

			auto& rjc = view.get<RevoluteJoint2DComponent>(e);
			if (rjc.Joint)
			{
				m_World->DestroyJoint((b2RevoluteJoint*)rjc.Joint);
				rjc.Joint = nullptr;
			}
		}

		m_World->SetContactListener(nullptr);

		delete m_ContactListener;
		delete m_World;
		m_ContactListener = nullptr;
		m_World = nullptr;
	}

}