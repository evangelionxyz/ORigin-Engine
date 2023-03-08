// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game
{
    public static class GlobalVariable
    {
        public static Vector3 translation = new Vector3();
    }

    public class Player : Entity
    {
        public TransformComponent Transform;
        public Rigidbody2DComponent Rigidbody2D;

        public float Speed;
        public Vector3 Vector;

        void OnCreate()
        {
            Transform = GetComponent<TransformComponent>();
            Rigidbody2D = GetComponent<Rigidbody2DComponent>();
        }

        void OnUpdate(float deltaTime)
        {
            float speed = 5.0f;
            Vector3 velocity = Vector3.Zero;

            if (Translation.Y < 5.0f)
                if (Input.IsKeyPressed(KeyCode.Space))
                    velocity.Y = 3.0f;

            if (Input.IsKeyPressed(KeyCode.A))
                velocity.X = -1.0f;
            else if (Input.IsKeyPressed(KeyCode.D))
                velocity.X = 1.0f;

            velocity *= speed * deltaTime;
            Rigidbody2D.ApplyLinearImpulse(velocity.XY, true);

            GlobalVariable.translation = Transform.Translation;
        }
    }

    public class Camera : Entity
    {
        private TransformComponent Transform;
        private Vector3 playerTranslation = new Vector3();
        void OnCreate()
        {
            Transform = GetComponent<TransformComponent>();
        }

        void OnUpdate(float deltaTime)
        {
            playerTranslation.X = GlobalVariable.translation.X;
            playerTranslation.Y = GlobalVariable.translation.Y;
            playerTranslation.Z = Transform.Translation.Z;

            Transform.Translation = playerTranslation;
        }
    }
}
