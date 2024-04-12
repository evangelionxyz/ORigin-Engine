using System;
using ORiginEngine;

namespace SandboxProject
{
  class CameraMovement : Entity
  {
    private Entity player;
    private Vector2 position;

    private float yOffset = 1.0f;
    public float zoneSize = 1.2f;
    void OnCreate()
    {
      player = FindEntityByName("Player");

      if (player != null)
        position = new Vector2(player.Translation.X, player.Translation.Y + yOffset);
    }

    void OnUpdate(float deltaTime)
    {
      Vector2 minBounding = new Vector2(Translation.X - zoneSize, Translation.Y - zoneSize);
      Vector2 maxBounding = new Vector2(Translation.X + zoneSize, Translation.Y + zoneSize);

      if (player != null)
      {
        Vector2 pPos = player.Translation.XY;

        if (pPos.X < minBounding.X)
          position.X -= 3.0f * (minBounding.X - pPos.X) * deltaTime;
        else if (pPos.X > maxBounding.X)
          position.X += 3.0f * (pPos.X - maxBounding.X) * deltaTime;

        float yMoveSpeed = pPos.Y - Translation.Y;
        if (System.Math.Abs(yMoveSpeed) > 1)
          yMoveSpeed *= 5.0f;

        if (pPos.Y > maxBounding.Y)
          position.Y += yMoveSpeed * deltaTime;
        else if (pPos.Y + yOffset < Translation.Y)
          position.Y += yMoveSpeed * deltaTime;

        Translation = new Vector3(position, Translation.Z);
      }
    }
  }
}
