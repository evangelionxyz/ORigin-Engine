// Copyright (c) 2023 Evangelion Manuhutu | ORigin Engine

using System;
using ORiginEngine;

namespace Game
{
    public class Circle : Entity
    {
        private CircleRendererComponent circleRenderer2DC;

        public void OnCreate()
        {
            circleRenderer2DC = GetComponent<CircleRendererComponent>();
        }

        public void OnUpdate(float deltaTime)
        {

        }
    }
}
