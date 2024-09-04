#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{

    VDOBB box;
    void init() override
    {
        box.setHalfExtents(VDVector3(0.5, 1, 1.5));
       // box.setPosition({ 4,4,4 });
    }

    void update(float dt) override
    {
        Scene::update(dt);
        box.rotation = VDQuaternion::fromEulerAngles(VDVector3(dt * 0.1, dt, dt * 0.7)) * box.rotation;
        box.rotation.normalize();
        box.frame = box.rotation.toFrame();
        box.setLowAndHigh();
    }

    void draw(float dt) override
    {
        drawBox(box, colorWhite);
        drawBox(box, colorCyan, false);
        drawAABB(box, colorGreen);
        drawBoxFrame(box, 3.0f);
    }
};


int main(void)
{
    initApplication();
    HellBoxScene scene;
    runApplication(&scene);
    return 0;
}