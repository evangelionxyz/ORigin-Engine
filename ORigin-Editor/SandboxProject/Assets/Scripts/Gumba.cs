
using ORiginEngine;

namespace Game
{
    public class Gumba : Entity
    {
        public float speed = 5.0f;
        public float Distance = 2.0f;

        private float increment = 1.0f;
        private float xCenter;
        private float maxDistance;
        private float minDistance;
        private bool mati = false;

        private Rigidbody2DComponent rb2d;
        private AnimationComponent anim;

        private float xMovement = 0.0f;

        public void OnCreate()
        {
            xCenter = Translation.X;

            maxDistance = Translation.X + Distance;
            minDistance = Translation.X - Distance;

            rb2d = GetComponent<Rigidbody2DComponent>();
            anim = GetComponent<AnimationComponent>();
            anim.ActiveState = "Idle";
        }

        private bool IsContactWithPlayer()
        {
            return rb2d.IsContactWithTag("Player");
        }

        public void OnUpdate(float deltaTime)
        {
            xMovement = 0.0f;

            if (xCenter >= maxDistance)
            {
                increment = -1.0f;
            }
            else if (xCenter <= minDistance)
            {
                increment = 1.0f;
            }

            xMovement += increment * speed * deltaTime;

            // 0, -0.130
            // 0.5, 0.250

            if (IsContactWithPlayer())
            {
                xMovement = 0.0f;
                anim.ActiveState = "Gepeng";
                mati = true;

            }
            else
            {
                anim.ActiveState = "Idle";
            }    

            rb2d.ApplyForceToCenter(new Vector2(xMovement, Translation.Y), true);
            xCenter = Translation.X;

            
        }
    }
}
