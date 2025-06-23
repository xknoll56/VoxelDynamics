#include "Utils.h"
#include "Application.h"

struct HelloRaycastScene : Scene
{
    VDSimulation sim;
    VDAgentController* pController;
    std::vector<InstanceBuffer> ibs;
	VDVector3 rayStart = VDVector3(0, 10, 0);
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
		movePositionWithArrows(camera, rayStart, dt, 5.0f);

    }

    void raycast(VDSpace* pSpace, VDVector3 from, VDVector3 dir, VDContactInfo& contactPoint)
    {
        VDGrid* pGrid = pSpace->getGrid(from);
    }

    void draw(float dt) override
    {
        drawPoint(rayStart, colorRed);
		VDGrid* cur = sim.space.getGrid(camera.position);
        drawGrid(cur, colorYellow);
        VDContactInfo contactPoint;
        if(cur->raycast(rayStart, VDVector3::down()+VDVector3::back(), 10000.0f, contactPoint))
        {
            drawPoint(contactPoint.point, colorGreen);
            drawLine(rayStart, contactPoint.point, colorGreen);
        }

        for (int i = 0; i < ibs.size(); i++)
        {
            drawInstanceBuffer(ibs[i], texArr);
        }
    }
};


int main(void)
{
    initApplication();
    HelloRaycastScene scene;
    runApplication(&scene);
    return 0;
}