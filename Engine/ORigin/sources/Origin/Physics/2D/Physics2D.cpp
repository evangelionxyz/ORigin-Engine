// Copyright (c) Evangelion Manuhutu | ORigin Engine

#include "pch.h"
#include "Physics2D.h"
#include "Origin/Scene/Entity.h"
#include "Origin/Scene/Components/Components.h"

#include <glm/glm.hpp>

namespace origin {

    static b2BodyType Box2DBodyType(Rigidbody2DComponent::BodyType type)
    {
        switch (type)
        {
        case Rigidbody2DComponent::BodyType::Static:    return b2_staticBody;
        case Rigidbody2DComponent::BodyType::Dynamic:   return b2_dynamicBody;
        case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
        }

        OGN_CORE_ASSERT(false, "Unkown Body Type");
        return b2_staticBody;
    }

    Physics2D::Physics2D(Scene* scene)
        : m_Scene(scene)
    {
        OGN_PROFILER_PHYSICS();
    }

    void Physics2D::CreateRevoluteJoint(RevoluteJoint2DComponent *joint, b2BodyId body_id)
    {
        OGN_PROFILER_PHYSICS();

        Entity connected_entity              = m_Scene->GetEntityWithUUID(joint->ConnectedBodyID);
        Rigidbody2DComponent &connected_body = connected_entity.GetComponent<Rigidbody2DComponent>();

        OGN_CORE_ASSERT(b2Body_IsValid(connected_body.BodyId), "[Physics 2D] Invalid connected body id");

        b2RevoluteJointDef joint_def = b2DefaultRevoluteJointDef();
        joint_def.enableSpring   = joint->EnableSpring;
        joint_def.collideConnected = joint->CollideConnected;
        joint_def.dampingRatio   = joint->SpringDampingRatio;
        joint_def.enableLimit    = joint->EnableLimit;
        joint_def.maxMotorTorque = joint->MaxMotorTorque;
        joint_def.motorSpeed     = joint->MotorSpeed;
        joint_def.enableMotor    = joint->EnableMotor;
        joint_def.bodyIdA        = body_id;
        joint_def.bodyIdB        = connected_body.BodyId;
        joint_def.localAnchorA   = b2Vec2(joint->AnchorPoint.x, joint->AnchorPoint.y);
        joint_def.localAnchorB   = b2Vec2(joint->AnchorPointB.x, joint->AnchorPointB.y);
        joint_def.lowerAngle     = joint->LowerAngle;
        joint_def.upperAngle     = joint->UpperAngle;

        joint->JointId = b2CreateRevoluteJoint(m_WorldId, &joint_def);
    }

    void Physics2D::CreateBoxCollider(BoxCollider2DComponent *box_collider, b2BodyId body_id, b2Vec2 size)
    {
        OGN_PROFILER_PHYSICS();
        box_collider->CurrentSize = { size.x, size.y };
        const b2Polygon box_shape = b2MakeBox(size.x, size.y);
        b2ShapeDef shape_def      = b2DefaultShapeDef();
        shape_def.density         = box_collider->Density;
        shape_def.friction        = box_collider->Friction;
        shape_def.restitution     = box_collider->Restitution;
        shape_def.isSensor        = box_collider->IsSensor;

        box_collider->ShapeId = b2CreatePolygonShape(body_id, &shape_def, &box_shape);
    }

    void Physics2D::CreateCircleCollider(CircleCollider2DComponent *circle, b2BodyId body_id, float radius)
    {
        OGN_PROFILER_PHYSICS();
        b2Circle circle_shape;
        circle_shape.center = { circle->Offset.x, circle->Offset.y };
        circle_shape.radius = radius;

        b2ShapeDef shape_def = b2DefaultShapeDef();
        shape_def.density     = circle->Density;
        shape_def.friction    = circle->Friction;
        shape_def.restitution = circle->Restitution;
        shape_def.isSensor    = circle->IsSensor;

        circle->ShapeId = b2CreateCircleShape(body_id, &shape_def, &circle_shape);
    }

    void Physics2D::Simulate(float deltaTime)
    {
        OGN_PROFILER_PHYSICS();

        const i32 step_count = 4;
        b2World_Step(m_WorldId, deltaTime, step_count);

        // Retrieve transform from Box2D
        auto rb2dView = m_Scene->m_Registry.view<Rigidbody2DComponent>();
        for (entt::entity e : rb2dView)
        {
            Entity entity{ e, m_Scene };

            auto &transform = entity.GetComponent<TransformComponent>();
            auto &body = entity.GetComponent<Rigidbody2DComponent>();
            auto &idc = entity.GetComponent<IDComponent>();

            if (entity.HasComponent<RevoluteJoint2DComponent>())
            {
                RevoluteJoint2DComponent &joint = entity.GetComponent<RevoluteJoint2DComponent>();
                b2RevoluteJoint_EnableLimit(joint.JointId, joint.EnableLimit);
                b2RevoluteJoint_SetMaxMotorTorque(joint.JointId, joint.MaxMotorTorque);
                b2RevoluteJoint_EnableMotor(joint.JointId, joint.EnableMotor);
                b2RevoluteJoint_SetMotorSpeed(joint.JointId, joint.MotorSpeed);
                b2RevoluteJoint_EnableSpring(joint.JointId, joint.EnableSpring);
                b2RevoluteJoint_SetSpringDampingRatio(joint.JointId, joint.SpringDampingRatio);
            }

            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                BoxCollider2DComponent &box_collider = entity.GetComponent<BoxCollider2DComponent>();
                b2Shape_SetDensity(box_collider.ShapeId, box_collider.Density);
                b2Shape_SetFriction(box_collider.ShapeId, box_collider.Friction);
                b2Shape_SetRestitution(box_collider.ShapeId, box_collider.Restitution);
                b2Vec2 size = { box_collider.Size.x * transform.WorldScale.x, box_collider.Size.y * transform.WorldScale.y };
                if (box_collider.CurrentSize.x != size.x || box_collider.CurrentSize.y != size.y)
                {
                    box_collider.CurrentSize = { size.x, size.y };
                    const b2Polygon box_shape = b2MakeBox(size.x, size.y);
                    b2Shape_SetPolygon(box_collider.ShapeId, &box_shape);
                }
            }

            if (entity.HasComponent<CircleCollider2DComponent>())
            {
                CircleCollider2DComponent &circle_collider = entity.GetComponent<CircleCollider2DComponent>();
                b2Shape_SetDensity(circle_collider.ShapeId, circle_collider.Density);
                b2Shape_SetFriction(circle_collider.ShapeId, circle_collider.Friction);
                b2Shape_SetRestitution(circle_collider.ShapeId, circle_collider.Restitution);
                b2Circle circle_shape = b2Shape_GetCircle(circle_collider.ShapeId);

                if (circle_collider.Radius != circle_shape.radius || circle_collider.Offset.x != circle_shape.center.x || circle_collider.Offset.y != circle_shape.center.y)
                {
                    circle_shape.radius = circle_collider.Radius;
                    circle_shape.center = { circle_collider.Offset.x, circle_collider.Offset.y };
                    b2Shape_SetCircle(circle_collider.ShapeId, &circle_shape);
                }
            }

            b2Vec2 position = b2Body_GetPosition(body.BodyId);
            b2Rot rotation = b2Body_GetRotation(body.BodyId);

            transform.WorldTranslation.x = position.x;
            transform.WorldTranslation.y = position.y;
            transform.WorldRotation      = glm::quat({ 0.0f, 0.0f, b2Rot_GetAngle(rotation) });
            transform.Translation        = transform.WorldTranslation;
            transform.Rotation           = transform.WorldRotation;

            if (idc.Parent)
            {
                auto &ptc = m_Scene->GetEntityWithUUID(idc.Parent).GetComponent<TransformComponent>();

                // rotate local translation by parent rotation and add to parent translation
                glm::vec3 rotatedLocalPos = ptc.WorldRotation * transform.Translation;
                transform.WorldTranslation = rotatedLocalPos + ptc.WorldTranslation;

                // combine parent and local rotations
                transform.WorldRotation = ptc.WorldRotation * transform.Rotation;
                transform.WorldScale = transform.Scale * ptc.WorldScale;
            }
        }
    }

    void Physics2D::OnSimulationStart()
    {
        OGN_PROFILER_PHYSICS();

        b2WorldDef world_def = b2DefaultWorldDef();
        m_WorldId = b2CreateWorld(&world_def);

        auto view = m_Scene->m_Registry.view<TransformComponent, Rigidbody2DComponent>();
        for (entt::entity e : view)
        {
            Entity entity = { e, m_Scene };
            auto [tc, body] = view.get<TransformComponent, Rigidbody2DComponent>(e);

            b2BodyDef bodyDef       = b2DefaultBodyDef();
            bodyDef.position        = { tc.WorldTranslation.x, tc.WorldTranslation.y };
            bodyDef.rotation        = b2MakeRot(glm::eulerAngles(tc.WorldRotation).z);
            bodyDef.gravityScale    = body.GravityScale;
            bodyDef.userData        = static_cast<void *>(&e);
            bodyDef.type            = Box2DBodyType(body.Type);
            bodyDef.linearDamping   = body.LinearDamping;
            bodyDef.angularDamping  = body.AngularDamping;
            bodyDef.enableSleep     = body.EnableSleep;
            bodyDef.isAwake         = body.IsAwake;
            bodyDef.isBullet        = body.IsBullet;
            bodyDef.isEnabled       = body.IsEnabled;
            bodyDef.fixedRotation   = body.FixedRotation;
            bodyDef.linearVelocity  = { body.LinearVelocity.x, body.LinearVelocity.y };
            bodyDef.angularVelocity = body.AngularVelocity;

            body.BodyId             = b2CreateBody(m_WorldId, &bodyDef);

            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
                CreateBoxCollider(&bc2d, body.BodyId, b2Vec2(bc2d.Size.x * tc.WorldScale.x, bc2d.Size.y * tc.WorldScale.y));
                b2Shape_SetUserData(bc2d.ShapeId, (void *)&e);
            }

            if (entity.HasComponent<CircleCollider2DComponent>())
            {
                auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
                float radius = tc.WorldScale.x * cc2d.Radius;
                CreateCircleCollider(&cc2d, body.BodyId, radius);
                b2Shape_SetUserData(cc2d.ShapeId, (void *)&e);
            }
        }

        // create joint after rigidbody attached
        // we need second body id
        for (entt::entity e : view)
        {
            Entity entity = { e, m_Scene };
            auto [tc, body] = view.get<TransformComponent, Rigidbody2DComponent>(e);

            if(entity.HasComponent<RevoluteJoint2DComponent>())
            {
                RevoluteJoint2DComponent& joint = entity.GetComponent<RevoluteJoint2DComponent>();
                if (joint.ConnectedBodyID != 0)
                {
                    CreateRevoluteJoint(&joint, body.BodyId);
                    b2Joint_SetUserData(joint.JointId, static_cast<void *>(&e));
                }
            }
        }
    }

    void Physics2D::OnInstantiateEntity(Entity entity)
    {
        if (entity.HasComponent<Rigidbody2DComponent>())
        {
            TransformComponent &tc     = entity.GetComponent<TransformComponent>();
            Rigidbody2DComponent &body = entity.GetComponent<Rigidbody2DComponent>();
            entt::entity handle        = entity.GetHandle();

            b2BodyDef bodyDef      = b2DefaultBodyDef(); 
            bodyDef.position       = { tc.WorldTranslation.x, tc.WorldTranslation.y };
            bodyDef.rotation       = b2MakeRot(glm::eulerAngles(tc.WorldRotation).z);
            bodyDef.gravityScale   = body.GravityScale;
            bodyDef.userData       = static_cast<void *>(&handle);
            bodyDef.type           = Box2DBodyType(body.Type);
            bodyDef.linearDamping  = body.LinearDamping;
            bodyDef.angularDamping = body.AngularDamping;
            bodyDef.enableSleep    = body.EnableSleep;
            bodyDef.isAwake        = body.IsAwake;
            bodyDef.isBullet       = body.IsBullet;
            bodyDef.isEnabled      = body.IsEnabled;
            bodyDef.fixedRotation  = body.FixedRotation;
            bodyDef.linearVelocity = { body.LinearVelocity.x, body.LinearVelocity.y };
            bodyDef.angularVelocity = body.AngularVelocity;

            body.BodyId = b2CreateBody(m_WorldId, &bodyDef);

            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                BoxCollider2DComponent &box_collider = entity.GetComponent<BoxCollider2DComponent>();
                CreateBoxCollider(&box_collider, body.BodyId, b2Vec2(box_collider.Size.x * tc.WorldScale.x, box_collider.Size.y * tc.WorldScale.y));
                b2Shape_SetUserData(box_collider.ShapeId, static_cast<void *>(&handle));
            }

            if (entity.HasComponent<CircleCollider2DComponent>())
            {
                CircleCollider2DComponent &circle_collider = entity.GetComponent<CircleCollider2DComponent>();
                float radius = tc.WorldScale.x * circle_collider.Radius;
                CreateCircleCollider(&circle_collider, body.BodyId, radius);
                b2Shape_SetUserData(circle_collider.ShapeId, static_cast<void *>(&handle));
            }

            if (entity.HasComponent<RevoluteJoint2DComponent>())
            {
                RevoluteJoint2DComponent  &joint = entity.GetComponent<RevoluteJoint2DComponent>();
                b2Vec2 pos = b2Body_GetPosition(body.BodyId);
                b2Vec2 anchorPoint = b2Vec2(pos.x + joint.AnchorPoint.x, pos.y + joint.AnchorPoint.y);
                if (joint.ConnectedBodyID != 0)
                {
                    joint.AnchorPoint = { pos.x + joint.AnchorPoint.x, pos.y + joint.AnchorPoint.y };
                    b2Joint_SetUserData(joint.JointId, static_cast<void *>(&handle));
                }       
            }
        }
    }

    void Physics2D::OnDestroyEntity(Entity entity)
    {
        if (entity.HasComponent<Rigidbody2DComponent>())
        {
            if (entity.HasComponent<RevoluteJoint2DComponent>())
            {
                RevoluteJoint2DComponent &joint = entity.GetComponent<RevoluteJoint2DComponent>();
                //b2DestroyJoint(joint.JointId);
            }

            if (entity.HasComponent<BoxCollider2DComponent>())
            {
                BoxCollider2DComponent &box_collider = entity.GetComponent<BoxCollider2DComponent>();
                b2DestroyShape(box_collider.ShapeId);
            }

            if (entity.HasComponent<CircleCollider2DComponent>())
            {
                CircleCollider2DComponent &circle_collider = entity.GetComponent<CircleCollider2DComponent>();
                b2DestroyShape(circle_collider.ShapeId);
            }

            Rigidbody2DComponent &body = entity.GetComponent<Rigidbody2DComponent>();
            b2DestroyBody(body.BodyId);
        }
    }

    void Physics2D::OnSimulationStop()
    {
        OGN_PROFILER_PHYSICS();

        auto view = m_Scene->m_Registry.view<Rigidbody2DComponent>();
        for (entt::entity e : view)
        {
            Entity entity = { e, m_Scene };
            Rigidbody2DComponent &body = view.get<Rigidbody2DComponent>(e);
            OnDestroyEntity(entity);
        }

        if (b2World_IsValid(m_WorldId))
            b2DestroyWorld(m_WorldId);
    }
}