// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game
{
    public class Player : Entity
    {
        private TransformComponent Transform;
        private Rigidbody2DComponent Rigidbody2D;
        private SpriteRenderer2DComponent SpriteRenderer2D;

        public float Speed = 5.0f;
        public float time;

        public Vector3 scale;
        public Entity entity;
        public float tilingFactor = 1.0f;

        void OnCreate()
        {
            scale = new Vector3(1.0f);
            Transform = GetComponent<TransformComponent>();
            Rigidbody2D = GetComponent<Rigidbody2DComponent>();
            SpriteRenderer2D = GetComponent<SpriteRenderer2DComponent>();

            time = 0.0f;
        }

        void OnUpdate(float deltaTime)
        {
            time += deltaTime;
            float speed = Speed;
            
            Scale = Transform.Scale;

            Vector3 velocity = Vector3.Zero;

            if (Translation.Y < 3.0f)
                if (Input.IsKeyPressed(KeyCode.Space))
                    velocity.Y = 3.0f;

            if (Input.IsKeyPressed(KeyCode.A))
                velocity.X = -1.0f;
            else if (Input.IsKeyPressed(KeyCode.D))
                velocity.X = 1.0f;

            velocity *= speed * deltaTime;
            Rigidbody2D.ApplyLinearImpulse(velocity.XY, true);
            scale.X += velocity.Y * 4.0f * deltaTime;
            Transform.Scale = scale;

            SpriteRenderer2D.TilingFactor = tilingFactor;
        }
    }
}
