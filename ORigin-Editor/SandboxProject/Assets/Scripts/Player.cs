// Copyright (c) Evangelion Manuhutu | ORigin Engine

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

        public float JumpHeight = 10.0f;
        public float MoveSpeed = 15.0f;
        public float JumpCoolDownTime = 0.3f;
        private float JumpCoolDown;
        private int jumpsRemaining = 2;
        float RotationY = 0.0f;

        private Vector2 velocity;

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

        private bool IsGrounded()
        {
            return rb2d.IsContactWithTag("Ground");
        }

        void OnUpdate(float deltaTime)
        {
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
                RotationY = velocity.X > 0 ? 0.0f : Math.Deg2Rad(-180.0f);
            }

            transform.Rotation = new Vector3(0.0f, RotationY, 0.0f);
            rb2d.ApplyLinearImpulseToCenter(velocity, true);

            if (IsGrounded())
            {
                jumpsRemaining = 2;
                
            }

            text.Text = "\nIs Contact With: " + rb2d.GetContactWithTag();
        }
    }
}
