using ORiginEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TestGame
{
    class Player : Entity
    {
        private Rigidbody rb;
        public Entity camera;
        public float Speed = 5.0f;

        void OnCreate()
        {
            rb = GetComponent<Rigidbody>();
        }

        void OnUpdate(float dt)
        {
            Vector3 forward = camera.GetForward().Normalized();
            Vector3 right = camera.GetRight().Normalized();

            Vector3 velocity = Vector3.Zero;
            if (Input.IsKeyPressed(KeyCode.A))
            {
                velocity += -right * Speed;
            }
            else if (Input.IsKeyPressed(KeyCode.D))
            {
                velocity += right * Speed;
            }
            if (Input.IsKeyPressed(KeyCode.W))
            {
                velocity += forward * Speed;
            }
            else if (Input.IsKeyPressed(KeyCode.S))
            {
                velocity += -forward * Speed;
            }

            rb.LinearVelocity = new Vector3(velocity.X, rb.LinearVelocity.Y, velocity.Z);
        }
    }
}
