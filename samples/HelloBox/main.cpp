#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{

    VDOBB box;
    void init() override
    {

    }

    void update(float dt) override
    {
        Scene::update(dt);
        box.rotate(VDQuaternion::fromEulerAngles(VDVector3(0, dt, 0)));
        box.setLowAndHigh();
    }

    void draw(float dt) override
    {
        drawBox(box, colorWhite);
        drawBox(box, colorCyan, GL_LINES);
        drawAABB(box.low, box.high, colorGreen);
    }
};


int main(void)
{
    initApplication();
    HellBoxScene scene;
    runApplication(&scene);
    return 0;
}