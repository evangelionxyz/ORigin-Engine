using ORiginEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;

namespace TestGame
{
    public class BlockSpawner : Entity
    {
        public Entity block;
        public Entity blockB;
        public Entity uiEntity;
        List<Entity> blockList;
        Vector3[] positions; 
        int size = 15;
        public int count = 0;
        public int total = 10;
        private float spawnTimer = 0.0f;
        private float spawnInterval = 0.05f; // 0.5 seconds
        private bool done = false;

        void OnCreate()
        {
            blockList = new List<Entity>();
        }

        void OnUpdate(float dt)
        {
            if (block != null && total < count && !done)
            {
                spawnTimer += dt;

                if (spawnTimer >= spawnInterval)
                {
                    Entity e = Instantiate(total % 2 == 0 ? block : blockB, positions[total]);
                    Rigidbody rb = e.GetComponent<Rigidbody>();
                    rb.SetPosition(positions[total], true);
                    blockList.Add(e);
                    total++;
                    done = total == count;
                    spawnTimer = 0.0f;
                }
            }

            if (done)
            {
                spawnTimer += dt;
                List<Entity> entityToRemove = new List<Entity>();
                foreach (Entity e in blockList)
                {
                    entityToRemove.Add(e);
                }

                if (spawnTimer >= spawnInterval)
                {
                    foreach (Entity e in entityToRemove)
                    {
                        Destroy(e);
                        blockList.Remove(e);
                        total--;
                        break;
                    }
                    spawnTimer = 0.0f;
                }

            }

            if(uiEntity != null)
            {
                UI ui = uiEntity.GetComponent<UI>();
                ui.SetTextString("CubeCount", $"{total} Total Cubes");
            }
        }
    }
}
