using ORiginEngine;
using System.Collections.Generic;

namespace TestGame
{
    public class PlayeMovement : Entity
    {
        private Rigidbody rb;
        public float Speed = 5.0f;
        public Entity bullet;
        public Entity camera;
        public Entity shootAudio;
        public Entity uiEntity;
        private List<Bullets> bullets;
        private float jumpRate = 1.0f;
        private float jumpTime = 0.0f;
        float rate = 0.05f;
        float playTime = 0.0f;
        float rotateY = 0.0f;
        float camRotate = -0.2f;
        float lastRotate = 0.0f;
        float uiRefresRate = 1.0f;
        float uiRefresh = 0.0f;
        int bulletCount = 0;
        void OnCreate()
        {
            playTime = rate;
            jumpTime = 0.0f;
            rb = GetComponent<Rigidbody>();
            bullets = new List<Bullets>();
        }

        void UpdateMouse(float dt)
        {
            if (Scene.IsFocusing())
            {
                rotateY += -Input.MouseDelta.X * dt * 0.1f;
                rb.SetEulerAngles(new Vector3(0.0f, rotateY, 0.0f), true);
                lastRotate = rotateY;
                if (camera != null)
                {
                    camRotate += -Input.MouseDelta.Y * dt * 0.05f;
                    Transform tc = camera.GetComponent<Transform>();
                    Vector3 rotate = new Vector3(camRotate, tc.EulerAngles.Y, 0.0f);
                    if (rotate.X > 1.8f)
                        rotate.X = 1.8f;
                    else if(rotate.X < -0.5f)
                        rotate.X = -0.5f;

                    tc.EulerAngles = rotate;
                }
            }

            if(!Scene.IsFocusing())
            {
                rb.SetEulerAngles(new Vector3(0.0f, lastRotate, 0.0f), true);
            }
        }

        void UpdateUI(float dt)
        {
            if(uiEntity != null)
            {
                uiRefresh -= dt;
                UI ui = uiEntity.GetComponent<UI>();
                if(uiRefresh <= 0.0f)
                {
                    float fps = 1.0f / dt;
                    ui.SetTextString("FPSCounter", $"{fps:F2} FPS");
                    uiRefresh = uiRefresRate;
                }

                ui.SetTextString("BulletCount", $"{bulletCount} Total bullets");
            }
        }

        void OnUpdate(float dt)
        {
            playTime -= dt;
            jumpTime -= dt;

            UpdateMouse(dt);
            UpdateUI(dt);

            Vector3 forward = GetForward().Normalized();
            Vector3 right = GetRight().Normalized();

            Vector3 velocity = Vector3.Zero;
            if(Input.IsKeyPressed(KeyCode.A))
            {
                velocity += -right * Speed;
            }
            else if (Input.IsKeyPressed(KeyCode.D))
            {
                velocity += right * Speed;
            }
            if (Input.IsKeyPressed(KeyCode.W))
            {
                velocity += forward * Speed;
            }
            else if (Input.IsKeyPressed(KeyCode.S))
            {
                velocity += -forward * Speed;
            }

            if (bullet != null)
            {
                if (Input.IsMouseDown(MouseCode.ButtonLeft) && playTime <= 0.0f)
                {
                    Entity e = Instantiate(bullet, new Vector3(Translation.X, Translation.Y + 2.0f, Translation.Z));
                    Rigidbody r = e.GetComponent<Rigidbody>();
                    Vector3 v = forward * 30.0f;
                    v.Y += 5.0f;
                    r.AddLinearVelocity(v);
                    bullets.Add(new Bullets(e));
                    playTime = rate;
                    bulletCount++;

                    if (shootAudio != null)
                    {
                        Audio au = shootAudio.GetComponent<Audio>();
                        au.PlayOverlapping();
                    }
                }

                List<Bullets> bulletsToRemove = new List<Bullets>();

                foreach (Bullets b in bullets)
                {
                    b.time -= dt;
                    if(b.time <= 0.0f)
                    {
                        bulletsToRemove.Add(b);
                    }
                }

                foreach(Bullets b in bulletsToRemove)
                {
                    Destroy(b.entity);
                    bullets.Remove(b);
                }
            }


            float impulse = 0.0f;
            if (Input.IsKeyPressed(KeyCode.Space) && jumpTime <= 0.0f)
            {
                impulse = 10.0f;
                rb.AddLinearVelocity(new Vector3(0.0f, impulse, 0.0f));
                jumpTime = jumpRate;
            }

            rb.LinearVelocity = new Vector3(velocity.X, rb.LinearVelocity.Y, velocity.Z);

            if(Input.IsKeyPressed(KeyCode.Space))
            {
                rb.AddForce(GetForward() * 500.0f);
            }
        }
    }
}
