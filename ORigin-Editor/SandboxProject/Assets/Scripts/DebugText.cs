using ORiginEngine;

namespace Game
{
    public class DebugText : Entity
    {
        public Vector2 positionOffset = new Vector2(-5.0f, 2.0f);
        private TransformComponent camPos;

        public void OnCreate()
        {
            camPos = FindEntityByName("Main Camera").GetComponent<TransformComponent>();
        }

        public void OnUpdate(float deltaTime)
        {
            Translation = new Vector3(
                positionOffset.X + camPos.Translation.X,
                positionOffset.Y + camPos.Translation.Y, 1.0f
            );
        }
    }
}
