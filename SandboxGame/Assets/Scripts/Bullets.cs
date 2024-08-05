using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ORiginEngine;

namespace TestGame
{
    public class Bullets
    {
        public Entity entity;
        public float time = 5.0f;
        public Bullets(Entity e)
        {
            this.entity = e;
        }
    }
}
