// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game
{
    public class Player : Entity
    {
        public Entity entity;
        public Entity textEntity;

        private TransformComponent transformC;
        private Rigidbody2DComponent rigidBody2DC;
        private SpriteRenderer2DComponent spriteRenderer2DC;
        private BoxCollider2DComponent boxCollider2DC;

        public float Speed = 5.0f;
        private float time = 0.0f;

        public float tilingfactor = 1.0f;
        public Vector3 scale = new Vector3(1.0f);

        public float Bc2Ddensity;

        void OnCreate()
        {
            transformC = GetComponent<TransformComponent>();
            rigidBody2DC = GetComponent<Rigidbody2DComponent>();
            spriteRenderer2DC = GetComponent<SpriteRenderer2DComponent>();
            boxCollider2DC = GetComponent<BoxCollider2DComponent>();

            Bc2Ddensity = boxCollider2DC.Density;

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

            if (Translation.Y < 3.0f)
                if (Input.IsKeyPressed(KeyCode.Space))
                    velocity.Y = 3.0f;

            if (Input.IsKeyPressed(KeyCode.A))
                velocity.X = -1.0f;
            else if (Input.IsKeyPressed(KeyCode.D))
                velocity.X = 1.0f;

            velocity *= speed * deltaTime;
            rigidBody2DC.ApplyLinearImpulse(velocity.XY, true);

            transformC.Scale = scale;
            spriteRenderer2DC.TilingFactor = tilingfactor;
            boxCollider2DC.Density = Bc2Ddensity;
        }
    }
}
