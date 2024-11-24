#include "Utils.h"
#include "Application.h"

struct HelloVoxelDynamicsScene : Scene
{
    VDSimulation sim;
    VDTriangle triangle;

    void init() override
    {
        triangle = VDTriangle({ 0,0,0 }, { 3,0,0 }, { 0,1,3 });
    }

    void update(float dt) override
    {
        Scene::update(dt);
    }

    void draw(float dt) override
    {
        drawTranslatedBox(VDVector3(2,2,2), {1,1,1});
        drawTriangle(triangle, VDVector3::right());
    }
};


int main(void)
{
    initApplication();
    HelloVoxelDynamicsScene scene;
    runApplication(&scene);
    return 0;
}