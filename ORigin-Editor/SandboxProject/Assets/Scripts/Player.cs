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
        private Rigidbody2DComponent rigidBody2DC;
        private SpriteRenderer2DComponent spriteRenderer2DC;
        private TransformComponent transformC;

        private float time;
        public float Speed = 5.0f;
        public float yAxis = 0.0f;

        public int keyPressedCount = 0;
<<<<<<< HEAD
        public float jumpTimeOut = 1.5f;
=======
        public float jumpTimeOut = 1.0f;
>>>>>>> f1659539fb0018360a4c9b046737157afdde0004

        void OnCreate()
        {
            transformC = GetComponent<TransformComponent>();
            rigidBody2DC = GetComponent<Rigidbody2DComponent>();

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

            float speed = Speed;

            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyPressed(KeyCode.Space))
            {
                if(keyPressedCount <= 1)
                    jumpAudio.Play();

                if (Translation.Y < 6.0f && jumpTimeOut > 0.0f)
<<<<<<< HEAD
                    velocity.Y = 4.0f;
=======
                    velocity.Y = 3.0f;

                keyPressedCount++;
                jumpTimeOut -= deltaTime;
            }
            else
            {
                keyPressedCount = 0;
                jumpTimeOut = 1.0f;
            }
                
>>>>>>> f1659539fb0018360a4c9b046737157afdde0004

                keyPressedCount++;
                jumpTimeOut -= deltaTime;
            }
            else
            {
                keyPressedCount = 0;
                jumpTimeOut = 1.0f;
            }
                

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
