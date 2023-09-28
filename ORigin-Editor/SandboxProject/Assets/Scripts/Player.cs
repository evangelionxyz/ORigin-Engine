// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game
{
    public class Player : Entity
    {
        private TransformComponent transform;
        private Rigidbody2DComponent rb2d;
        private AnimationComponent anim;

        public float JumpHeight = 0.5f;
        public float FallSpeed = 10.0f;
        public float Speed;
        public float Increment;

        private Vector2 velocity;

        float Deg2Rad(float x)
        {
            // 180 / 3,141592653589793238462643
            return x * 0.01745329f;
        }

        void OnCreate()
        {
            transform = GetComponent<TransformComponent>();
            rb2d = GetComponent<Rigidbody2DComponent>();
            anim = GetComponent<AnimationComponent>();
        }

        void OnUpdate(float deltaTime)
        {
            velocity = Vector2.Zero;

            if (Input.IsKeyPressed(KeyCode.Space))
                velocity.Y += JumpHeight * deltaTime;
            else
                velocity.Y -= Increment * FallSpeed * deltaTime;

            if (Input.IsKeyPressed(KeyCode.D))
            {
                velocity.X += Increment * Increment * deltaTime;
                transform.Rotation = new Vector3(0.0f, 0.0f, 0.0f);
            }
            else if (Input.IsKeyPressed(KeyCode.A))
            {
                velocity.X -= Increment * Increment * deltaTime;
                transform.Rotation = new Vector3(0.0f, Deg2Rad(-180.0f), 0.0f);
            }

            if (Input.IsKeyPressed(KeyCode.B))
                anim.ActiveState = "Fireball";
            else
                anim.ActiveState = "Mario";

            rb2d.ApplyLinearImpulse(velocity, new Vector2(0.0f, 0.0f), true);
        }
    }
}
