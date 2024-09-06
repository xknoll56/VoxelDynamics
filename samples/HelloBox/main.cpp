#include "Utils.h"
#include "Application.h"

struct HellBoxScene : Scene
{

    VDOBB box;
    VDImplicitPlane plane;
    void init() override
    {
        box.setHalfExtents(VDVector3(0.5, 1, 1.5));
        box.setPosition({ 4,4,4 });
        plane = VDImplicitPlane(VDVector3(), VDVector3(1, 1, 1), 5.0f, 8.0f, 0.45f);
    }

    void update(float dt) override
    {
        Scene::update(dt);
        box.rotate(VDQuaternion::fromEulerAngles(VDVector3(dt * 0.1, dt, dt * 0.7)));
  
        box.setLowAndHigh();
        box.setVertices();
    }

    void draw(float dt) override
    {
        drawBox(box, colorWhite);
        drawBox(box, colorCyan, false);
        drawAABB(box, colorGreen);
        drawBoxFrame(box, 3.0f);

        VDContactInfo point;
        if (VDRayCastImplicitPlane(VDVector3(1, 10, 5), VDVector3(0, -1,0), plane, point))
        {
            drawLine(VDVector3(0, 10, 0), point.point, colorGreen);
            drawBox(point.point, VDVector3(), VDVector3::one() * 0.1f, colorGreen);
        }
        drawImplicitPlane(plane, colorWhite);
    }
};


int main(void)
{
    initApplication();
    HellBoxScene scene;
    runApplication(&scene);
    return 0;
}