#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{


    VDOBB box;
    VDEdge edge;

    void init() override
    {
        box.setHalfExtents(VDVector3(0.5, 1, 1.5));
		edge = VDEdge(VDVector3(-2, -2, -2), VDVector3(2, 2, 2));
    }

    void update(float dt) override
    {
        Scene::update(dt);
		box.setRotation(VDQuaternion::fromAngleAxis(VDVector3(0, 1, 0), elapsedTime));
    }

    void draw(float dt) override
    {
        drawBox(box, colorWhite);
        drawBox(box, colorCyan, false);
        drawAABB(box, colorGreen);
        drawEdge(edge, colorRed);
    }
};


int main(void)
{
    initApplication();
    HellBoxScene scene;
    runApplication(&scene);
    return 0;
}