using System;
using ORiginEngine;

namespace SandboxProject
{
  class PlayerMovement : Entity
  {
    private Rigidbody2DComponent rb2d;
    private SpriteRenderer2DComponent src;
    private BoxCollider2DComponent box;
    private SpriteAnimationComponent anim;

    private float jumpInterval;
    private float jumpCooldown;
    private int jumpCount;
    private bool canJump;
    private float Speed;
    private float JumpHeight;

    public void OnCreate()
    {
      jumpInterval = 0.5f;
      jumpCooldown = jumpInterval;
      jumpCount = 0;
      canJump = true;
      Speed = 3.0f;
      JumpHeight = 3.5f;

      rb2d = GetComponent<Rigidbody2DComponent>();
      src = GetComponent<SpriteRenderer2DComponent>();
      box = GetComponent<BoxCollider2DComponent>();
      anim = GetComponent<SpriteAnimationComponent>();
    }
    public void OnUpdate(float dt)
    {
      anim.ActiveState = "Idle";

      Vector2 v = new Vector2(0.0f);
      if (Input.IsKeyPressed(KeyCode.A))
      {
        anim.ActiveState = "Walk";
        src.FlipX = true;
        v.X -= Speed;
      }
      else if (Input.IsKeyPressed(KeyCode.D))
      {
        anim.ActiveState = "Walk";
        src.FlipX = false;
        v.X += Speed;
      }

      if (Input.IsKeyPressed(KeyCode.Space) && Translation.Y < 3.5f)
      {
        if (canJump)
        {
          if (jumpCount < 2)
          {
            jumpCooldown = jumpInterval;
            jumpCount++;
            canJump = false;
            rb2d.ApplyLinearImpulseToCenter(new Vector2(0.0f, jumpCount > 0 ? JumpHeight * jumpCount : jumpCount), false);
          }
        }
      }

      if (!canJump)
      {
        jumpCooldown -= dt;
        if (jumpCooldown <= 0.0f)
        {
          canJump = true;
        }
      }

      if (rb2d.IsContactWithTag("Floor") || rb2d.IsContactWithTag("Long Floor"))
      {
        canJump = true;
        jumpCount = 0;
        jumpCooldown = jumpInterval;
      }

      rb2d.Velocity = new Vector2(v.X, rb2d.Velocity.Y);
    }
  }
}
