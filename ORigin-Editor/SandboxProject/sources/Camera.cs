// Copyright (c) 2022 Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game
{
    public class Camera : Entity
    {
        private TransformComponent Transform;
        private Vector3 playerTranslation = new Vector3();
        void OnCreate()
        {
            Transform = GetComponent<TransformComponent>();
        }

        void OnUpdate(float deltaTime)
        {
            playerTranslation.X = GlobalVariable.translation.X;
            playerTranslation.Y = GlobalVariable.translation.Y;
            playerTranslation.Z = Transform.Translation.Z;

            Transform.Translation = playerTranslation;
        }
    }
}