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
        plane = VDImplicitPlane(VDVector3(), VDVector3(0, 1, 0), 10.0f, 10.0f, 0.0f);
        rayCastStart = VDVector3(3, 10, 2);
    }

    void update(float dt) override
    {
        Scene::update(dt);
        movePositionWithArrows(camera, rayCastStart, dt, 2.0f);
        box.setPosition(rayCastStart);
        box.rotate(VDQuaternion::fromEulerAngles(VDVector3(dt * 0.1, dt, dt * 0.7)));
  
        box.setLowAndHigh();
        box.setVertices();
    }

    bool VDCollisionBoxImplicitPlane(const VDOBB& box, const VDImplicitPlane& plane, VDManifold& manifold)
    {
        VDVector3 dp = plane.center - box.position;

    }

    void draw(float dt) override
    {
        drawBox(box, colorWhite);
        drawBox(box, colorCyan, false);
        drawAABB(box, colorGreen);
        drawBoxFrame(box, 3.0f);
        
        drawImplicitPlane(plane, colorWhite);

    }
};


int main(void)
{
    initApplication();
    HelloRaycastingScene scene;
    runApplication(&scene);
    return 0;
}