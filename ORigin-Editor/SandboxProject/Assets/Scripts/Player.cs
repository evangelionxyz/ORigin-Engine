// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game
{
    public class Player : Entity
    {
        public Entity entity;
        public Entity textEntity;
        private AudioComponent jumpAudio;
        private TransformComponent transform;
        private BoxCollider2DComponent bc2D;

        public float Speed = 2.0f;
        public float Increment = 10.0f;

        private float time;
        private int keyPressedCount = 0;

        private Vector2 velocity;
        private Vector2 translation;

        void OnCreate()
        {
            velocity = Vector2.Zero;
            translation = Vector2.Zero;

            transform = GetComponent<TransformComponent>();
            bc2D = GetComponent<BoxCollider2DComponent>();

            textEntity = FindEntityByName("TimeStep");
            jumpAudio = FindEntityByName("JumpAudio").GetComponent<AudioComponent>();
        }

        void OnUpdate(float deltaTime)
        {
            time += deltaTime;

            if(textEntity != null)
            {
                TextComponent text = textEntity.GetComponent<TextComponent>();
                text.Text = "Time: " + time.ToString();
            }

            if (Input.IsKeyPressed(KeyCode.Space))
            {
                if(keyPressedCount < 1)
                    jumpAudio.Play();

                velocity.Y += Increment;

                keyPressedCount++;
            }
            else
            {
                velocity.Y -= Increment;
                keyPressedCount = 0;
            }

            velocity.X += Increment;

            velocity *= Speed * deltaTime;
            translation += velocity;

            transform.Translation = new Vector3(translation, 0.0f);
        }
    }
}
