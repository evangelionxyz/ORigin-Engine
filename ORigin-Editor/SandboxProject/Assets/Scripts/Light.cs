// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

using ORiginEngine;

namespace Game
{
    public class Light : Entity
    {
        public float Speed = 0.5f;
        public float distance = 10.0f;

        private float move = 0.0f;
        private float increment = 1.0f;

        public void OnCreate()
        {
        }

        public void OnUpdate(float deltaTime)
        {
            Translation = new Vector3(move, Translation.Y, Translation.Z);
            if (move > distance)
                increment = -1.0f;
            else if (move < -distance)
                increment = 1.0f;

            move += increment * Speed * deltaTime;
        }
    }
}
