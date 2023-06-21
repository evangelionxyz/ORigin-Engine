// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using ORiginEngine;

namespace Game
{
    public class Camera : Entity
    {
        private Entity player;
        public float distance = 8.0f;

        void OnCreate()
        {
            player = FindEntityByName("Player");
        }

        void OnUpdate(float deltaTime)
        {
            if(player != null)
                Translation = new Vector3(player.Translation.XY, distance);
        }
    }
}