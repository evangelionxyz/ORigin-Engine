using System;
using ORiginEngine;

namespace SandboxProject.Assets.Scripts
{
  class PlayerMovement : Entity
  {
    private Rigidbody2DComponent rb2d;
    private SpriteRenderer2DComponent src;
    public float Speed = 8.0f;
    public float JumpHeight = 5.0f;

    public void OnCreate()
    {
      rb2d = GetComponent<Rigidbody2DComponent>();
    }
    public void OnUpdate(float dt)
    {

      Vector2 v = new Vector2(0.0f);
      if (Input.IsKeyPressed(KeyCode.A))
        v.X -= Speed;
      else if (Input.IsKeyPressed(KeyCode.D))
        v.X += Speed;

      if (Input.IsKeyPressed(KeyCode.Space))
        rb2d.ApplyForceToCenter(new Vector2(0.0f, JumpHeight), true);

      rb2d.Velocity = new Vector2(v.X, rb2d.Velocity.Y);
     
    }
  }
}
