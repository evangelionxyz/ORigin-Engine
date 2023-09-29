using System;
using ORiginEngine;

namespace Game
{
    public class Camera : Entity
    {
        private Entity player;
        private Vector2 position;

        void OnCreate()
        {
            player = FindEntityByName("Player");
            position = Vector2.Zero;
        }

        void OnUpdate(float deltaTime)
        {
            Vector2 minBounding = new Vector2(Translation.X - 2.0f, 0.0f);
            Vector2 maxBounding = new Vector2(Translation.X + 2.0f, Translation.Y + 1.0f);

            if(player != null)
            {
                if (player.Translation.X < minBounding.X)
                    position.X -= 3.0f * (minBounding.X - player.Translation.X) * deltaTime;
                else if (player.Translation.X > maxBounding.X)
                    position.X += 3.0f * (player.Translation.X - maxBounding.X) * deltaTime;

                if (player.Translation.Y > maxBounding.Y)
                    position.Y += 3.0f * deltaTime;
                else if(player.Translation.Y < Translation.Y)
                    position.Y -= 3.0f * deltaTime;

                if (position.Y <= 0.0f)
                    position.Y = 0.0f;


                Translation = new Vector3(position, Translation.Z);
            }
        }
    }
}
