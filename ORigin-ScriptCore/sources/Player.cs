// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using ORiginEngine;
using static System.Runtime.CompilerServices.RuntimeHelpers;

namespace Game
{
    public class Player : Entity
    {
        public float Speed = 5.0f;

        void OnCreate()
        {
            System.Console.WriteLine($"C# Says: Player.OnCreate {ID}");
        }

        void OnUpdate(float time)
        {
            float speed = 2.0f;
            Vector3 velocity = Vector3.Zero();

            if (Input.IsKeyPressed(KeyCode.W))
                velocity.Y = 1.0f;
            if (Input.IsKeyPressed(KeyCode.S))
                velocity.Y = -1.0f;
            if (Input.IsKeyPressed(KeyCode.A))
                velocity.X = -1.0f;
            if (Input.IsKeyPressed(KeyCode.D))
                velocity.X = 1.0f;

            velocity *= speed;

            Vector3 translation = Translation;
            translation += velocity * time;
            Translation = translation;
        }
    }
}
