#include "Utils.h"
#include "Application.h"

struct HelloRaycastingScene : Scene
{

    VDOBB box;
    VDImplicitPlane plane;
    VDVector3 rayCastStart;
    void init() override
    {
        box.setHalfExtents(VDVector3(0.5, 1, 1.5));
        box.setPosition({ 4,4,4 });
        plane = VDImplicitPlane(VDVector3(), VDVector3(1, 1, 1), 5.0f, 8.0f, 0.45f);
        rayCastStart = VDVector3(3, 10, 2);
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
        VDAABB rayAABBTest({ -2,-2,-2 }, { 2,2,2 });
        drawSolidAABB(rayAABBTest, colorWhite);
        
        movePositionWithArrows(camera, rayCastStart, dt, 2.0f);
        if (VDRayCastOBB(rayCastStart, VDNormalize(box.position - rayCastStart), box, point))
        {
            drawLine(rayCastStart, point.point, colorGreen);
            drawTranslatedBox(point.point, colorGreen, VDVector3::uniformScale(0.1f));
        }
    }
};


int main(void)
{
    initApplication();
    HelloRaycastingScene scene;
    runApplication(&scene);
    return 0;
}