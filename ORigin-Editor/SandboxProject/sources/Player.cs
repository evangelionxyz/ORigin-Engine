// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game
{
    public class Player : Entity
    {
        private TransformComponent Transform;
        private Rigidbody2DComponent Rigidbody2D;

        public float Speed = 5.0f;
        public float time;
        public Vector3 Vector;

        public Entity entity;

        void OnCreate()
        {
            Transform = GetComponent<TransformComponent>();
            Rigidbody2D = GetComponent<Rigidbody2DComponent>();
            time = 0.0f;
        }

        void OnUpdate(float deltaTime)
        {
            time += deltaTime;
            float speed = Speed;

            Vector3 velocity = Vector3.Zero;

            if (Translation.Y < 3.0f)
                if (Input.IsKeyPressed(KeyCode.Space))
                    velocity.Y = 3.0f;

            if (Input.IsKeyPressed(KeyCode.A))
                velocity.X = -1.0f;
            else if (Input.IsKeyPressed(KeyCode.D))
                velocity.X = 1.0f;

            Entity cameraEntity = FindEntityByName("Camera");
            if(cameraEntity != null)
            {
                Camera camera = cameraEntity.As<Camera>();
                if (Input.IsKeyPressed(KeyCode.Q))
                    camera.distance -= 2.0f * speed * deltaTime;
                else if (Input.IsKeyPressed(KeyCode.E))
                    camera.distance += 2.0f * speed * deltaTime;
            }

            velocity *= speed * deltaTime;
            Rigidbody2D.ApplyLinearImpulse(velocity.XY, true);
        }
    }
}
