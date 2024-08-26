#include "Utils.h"
#include "Application.h"

struct HelloBodyScene : Scene
{
    VDSimulation sim;
    VDAgentController* pController;
    InstanceBuffer ib;
    std::vector<VDBody*> pBodyStack;
    VDBody* pControllable;
    void init() override
    {
        camera.position += VDVector3(0, 5, 0);
        sim = VDSimulation(50, { -25, 0, -25 }, 1, 1);
        for (int i = 0; i < 50; i++)
        {
            for (int j = 0; j < 50; j++)
            {
                sim.space.setVoxelOccupied({ float(i - 25), 0, float(j - 25) });
            }
        }
       ib = InstanceBuffer::instanceBufferFromChunk(*sim.space.grids[0].pChunk, 50 * 50);
       for (int i = 0; i < 5; i++)
       {
           for (int j = 0; j <= i; j++)
           {
               VDBody* pBody = sim.createAABBBody(VDAABB::fromMidPointAndHalfExtents(VDVector3::half(), VDVector3(j, 1.5+i, 0)), 1.0f);
               //pBody->setSleeping(true);
               pBodyStack.push_back(pBody);
           }
       }
       //pBody = sim.createAABBBody(VDAABB::fromMidPointAndHalfExtents(VDVector3::half(), VDVector3(0, 8, 0)), 1.0f);
       pControllable = sim.createAABBBody(VDAABB::fromMidPointAndHalfExtents(VDVector3::half(), VDVector3(0, 10, 0)), 1.0f);
    }

    void update(float dt) override
    {
        Scene::update(dt);
        sim.simulate(dt);
        if (keysDown[GLFW_KEY_SPACE])
        {
            pControllable->sleeping = false;
            pControllable->setPosition(camera.position);
            pControllable->clearForces();
            pControllable->deltaMomentums.insert(camera.forward * 20.0f);
            pControllable->momentum = VDVector3();
        }
    }

    void draw(float dt) override
    {
        drawInstanceBuffer(ib, texArr);
        for (int i = 0; i < pBodyStack.size(); i++)
        {
            if(pBodyStack[i]->sleeping)
                drawSolidAABB(*pBodyStack[i], { 1,0,0 });
            else
                drawSolidAABB(*pBodyStack[i], { 0,1,0 });
        }
        drawSolidAABB(*pControllable, { 1,1,1 });
    }
};


int main(void)
{
    initApplication();
    HelloBodyScene scene;
    runApplication(&scene);
    return 0;
}