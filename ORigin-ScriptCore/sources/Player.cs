// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using ORiginEngine;


namespace Game
{
    public class Player : Entity
    {
        private TransformComponent Transform;
        private Rigidbody2DComponent Rigidbody2D;

        void OnCreate()
        {
            Transform = GetComponent<TransformComponent>();
            Rigidbody2D = GetComponent<Rigidbody2DComponent>();
        }

        void OnUpdate(float deltaTime)
        {
            float speed = 10.0f;
            Vector3 velocity = Vector3.Zero;

            if(Translation.Y < 5.0f)
                if (Input.IsKeyPressed(KeyCode.Space))
                    velocity.Y = 3.0f;

            if (Input.IsKeyPressed(KeyCode.A))
                velocity.X = -1.0f;
            else if (Input.IsKeyPressed(KeyCode.D))
                velocity.X = 1.0f;

            velocity *= speed * deltaTime;
            Rigidbody2D.ApplyLinearImpulse(velocity.XY, true);
        }
    }
}
