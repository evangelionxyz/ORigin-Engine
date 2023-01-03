// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using ORiginEngine;

namespace Game
{
    public class Player : Entity
    {
        public float Speed = 5.0f;

        void OnCreate()
        {
            System.Console.WriteLine("C# Says: Player.OnCreate");
        }

        void OnUpdate(float time)
        {
            System.Console.WriteLine($"C# Says: Player.Update {time}");
        }
    }
}
