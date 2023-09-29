// Copyright (c) Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game
{
    public class Player : Entity
    {
        private AudioComponent JumpSound;
        private TransformComponent transform;
        private Rigidbody2DComponent rb2d;
        private AnimationComponent anim;
        private TextComponent text;

        public float JumpHeight;
        public float MoveSpeed;
        public float JumpCoolDownTime;
        private float JumpCoolDown;
        private int jumpsRemaining;
        float RotationY = 0.0f;

        private float time = 0.0f;

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
            JumpSound = FindEntityByName("Jump Sound").GetComponent<AudioComponent>();
            text = FindEntityByName("Text").GetComponent<TextComponent>();

            JumpCoolDown = JumpCoolDownTime;

            jumpsRemaining = 2;
        }

        void OnUpdate(float deltaTime)
        {
            time += deltaTime;
            text.Text = "Time: " + time.ToString();

            velocity = Vector2.Zero;

            if (Input.IsKeyPressed(KeyCode.Space))
            {
                if(jumpsRemaining > 1)
                {
                    if (JumpCoolDown <= 0.0f)
                    {
                        velocity.Y = JumpHeight;
                        JumpCoolDown = JumpCoolDownTime;
                        jumpsRemaining--;
                    }
                }
                
            }

            JumpCoolDown -= deltaTime;

            if (velocity.Y > 0)
            {
                JumpSound.Play();
                anim.ActiveState = "Jump";
            }
            else
            {
                anim.ActiveState = "Idle";
            }

            if (Input.IsKeyPressed(KeyCode.D))
                velocity.X += MoveSpeed * deltaTime;
            else if (Input.IsKeyPressed(KeyCode.A))
                velocity.X -= MoveSpeed * deltaTime;

            if (velocity.X != 0)
            {
                anim.ActiveState = "Walk";
                RotationY = velocity.X > 0 ? 0.0f : Deg2Rad(-180.0f);
            }

            transform.Rotation = new Vector3(0.0f, RotationY, 0.0f);
            rb2d.ApplyLinearImpulseToCenter(velocity, true);

            if (Translation.Y < -1.05f)
                jumpsRemaining = 2;
        }
    }
}
