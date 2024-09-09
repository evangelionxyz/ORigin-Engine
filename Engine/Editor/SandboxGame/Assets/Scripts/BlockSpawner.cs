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
        int count = 0;
        private int total = 0;
        private float spawnTimer = 0.0f;
        private float spawnInterval = 0.05f; // 0.5 seconds
        private bool done = false;

        void OnCreate()
        {
            blockList = new List<Entity>();
            positions = new Vector3[(size * 2) * (size * 2)];

            for (int x = -size; x < size; x++)
            {
                for (int z = -size; z < size; z++)
                {
                    // Correct the logic for setting positions
                    positions[count] = new Vector3(Mathf.Sin(x) * 2.0f, size + x + size * 4.0f, Mathf.Cos(z) * 2.0f);
                    count++;
                }
            }
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
