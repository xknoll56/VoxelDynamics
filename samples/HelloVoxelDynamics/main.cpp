#include "Utils.h"
#include "Application.h"

struct HelloVoxelDynamicsScene : Scene
{
    VDSimulation sim;
    VDAgentController* pController;
    std::vector<InstanceBuffer> ibs;
    void init() override
    {
        texArr.initCheckersTexture(0);
        sim = VDSimulation(20, { -20,-20,-20 }, 2, 2);
        for (int i = -10; i <= 10; i++)
        {
            for (int j = -10; j <= 10; j++)
            {
                sim.space.setVoxelOccupied({ (float)i, 0, (float)j });
            }
        }
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    sim.space.setChunkOccupied({ i,j,k });
                }
            }
        }
        ibs.reserve(8);
        for (int i = 0; i < 8; i++)
        {
            if (sim.space.grids[i].occupied)
            {
                ibs.push_back(InstanceBuffer::instanceBufferFromChunk(*sim.space.grids[i].pChunk, 400));
                sim.space.grids[i].pChunk->userData = &ibs[i];
            }
        }
        pController = sim.createAgentController(VDVector3(0, 2, 0), VDVector3(0.3, 0.8, 0.3), 3.0f);
    }

    void update(float dt) override
    {
        Scene::update(dt);
        moveAgentWithArrows(camera, *pController, dt, pController->speed);
        camera.position = pController->position + VDVector3(0, pController->halfExtents.y, 0);
        sim.simulate(dt);
		VDGrid* pGrid = sim.space.getGrid(camera.position);
		if (pGrid != nullptr)
		{
            VDVoxel* pVoxel = sim.space.getVoxel(camera.position + camera.forward * 2.5f);
            if (pVoxel)
            {
                drawVoxel(*pVoxel, { 0,0,1 });
                if (keysDown[GLFW_KEY_Q])
                {
                    VDGrid* PVoxelGrid = sim.space.grids[pVoxel->chunkIndex].pChunk;
                    InstanceBuffer* pBuffer = PVoxelGrid->userData;
                    if (!pVoxel->occupied)
                    {
                        pVoxel->occupied = true;
                        pBuffer->data.insertVoxel(pVoxel);
                        pBuffer->bind();
                        pBuffer->updateInstanceBuffer();
                    }
                }
                if (keysDown[GLFW_KEY_E])
                {
                    VDGrid* PVoxelGrid = sim.space.grids[pVoxel->chunkIndex].pChunk;
                    InstanceBuffer* pBuffer = PVoxelGrid->userData;
                    if (pVoxel->occupied)
                    {
                        pVoxel->occupied = false;
                        pBuffer->data.removeVoxel(pVoxel);
                        pBuffer->bind();
                        pBuffer->updateInstanceBuffer();
                    }
                }
            }
		}

    }

    void draw(float dt) override
    {
       // drawTranslatedBox(VDVector3(), {1,0,1});
        drawSpace(sim.space, { 0,1,0 });

        for (int i = 0; i < ibs.size(); i++)
        {
            drawInstanceBuffer(ibs[i], texArr);
        }
    }
};


int main(void)
{
    initApplication();
    HelloVoxelDynamicsScene scene;
    runApplication(&scene);
    return 0;
}