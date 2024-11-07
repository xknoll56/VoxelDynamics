#include "Utils.h"
#include "Application.h"

struct HelloVoxelDynamicsScene : Scene
{
    VDSimulation sim;

    void init() override
    {

    }

    void update(float dt) override
    {
        Scene::update(dt);
    }

    void draw(float dt) override
    {
        drawTranslatedBox(VDVector3(), {1,1,1});

    }
};


int main(void)
{
    initApplication();
    HelloVoxelDynamicsScene scene;
    runApplication(&scene);
    return 0;
}