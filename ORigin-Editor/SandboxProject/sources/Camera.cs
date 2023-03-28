// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using ORiginEngine;

namespace Game
{
    public class Camera : Entity
    {
        private Entity player;
        public float distance = 8.0f;
        private float increment = 1.0f;

        void OnCreate()
        {
            player = FindEntityByName("Player");
        }

        void OnUpdate(float deltaTime)
        {
            if(player != null)
                Translation = new Vector3(player.Translation.XY, distance);

            if (distance > 25.0f)
                increment = -1.0f;
            else if (distance < 8.0f)
                increment = 1.0f;

            distance += increment * 10.0f * deltaTime;
        }
    }
}