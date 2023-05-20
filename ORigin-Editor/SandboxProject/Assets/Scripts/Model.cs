// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using ORiginEngine;

namespace Game
{
    public class Model : Entity
    {
        public float rotationSpeed = 5.0f;
        public float increment = 1.0f;

        private float rotate = 0.0f;

        public void OnCreate()
        {
        }

        public void OnUpdate(float deltaTime)
        {
            Rotation = new Vector3(Rotation.X, rotate, Rotation.Z);
            rotate += increment * rotationSpeed * deltaTime;
        }
    }
}
