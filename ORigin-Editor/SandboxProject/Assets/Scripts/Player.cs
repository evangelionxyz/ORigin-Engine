// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game
{
    public class Player : Entity
    {
        public Entity entity;
        public Entity textEntity;

        private Rigidbody2DComponent rigidBody2DC;
        private SpriteRenderer2DComponent spriteRenderer2DC;
        private TransformComponent transformC;

        private float time;
        public float Speed = 5.0f;
        public float yAxis = 0.0f;

        void OnCreate()
        {
            transformC = GetComponent<TransformComponent>();
            rigidBody2DC = GetComponent<Rigidbody2DComponent>();

            textEntity = FindEntityByName("TimeStep");
        }

        void OnUpdate(float deltaTime)
        {
            time += deltaTime;

            if(textEntity != null)
            {
                TextComponent text = textEntity.GetComponent<TextComponent>();
                text.Text = "Time: " + time.ToString();
            }

            float speed = Speed;

            Vector3 velocity = Vector3.Zero;

            if (Translation.Y < 6.0f)
                if (Input.IsKeyPressed(KeyCode.Space))
                    velocity.Y = 4.0f;

            if (Input.IsKeyPressed(KeyCode.A))
                velocity.X = -1.0f;
            else if (Input.IsKeyPressed(KeyCode.D))
                velocity.X = 1.0f;

            velocity *= speed * deltaTime;
            rigidBody2DC.ApplyLinearImpulse(velocity.XY, true);

            yAxis = velocity.Y;
        }
    }
}
